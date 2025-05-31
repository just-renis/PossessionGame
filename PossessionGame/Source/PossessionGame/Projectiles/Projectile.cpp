#include "Projectile.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/AI/Base Data/AICharacterData.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraVariationData.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = Mesh;
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("movementComponent"));
	MovementComponent->InitialSpeed = 2000.f;
	MovementComponent->MaxSpeed = 2000.0f;
	MovementComponent->ProjectileGravityScale = 0.0f;
}

void AProjectile::SetupProjectile(const FProjectileData& InProjectileData, const FVector& Scale, const int32 InWhich)
{
	SetActorScale3D(Scale);
	ProjectileData = InProjectileData;
	Mesh->SetCollisionObjectType(ProjectileData.bIsPlayerProjectile ? ECC_GameTraceChannel1 : ECC_GameTraceChannel2);
	SetupData(ProjectileData.bIsPlayerProjectile, InWhich);
	if (!bHasBeenPooled)
	{
		if (const EVfx VfxName = AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, EVfxType::Trail); VfxName != EVfx::None)
		{
			const TObjectPtr<AWeapon> Weapon = ProjectileData.GetProprietor()->GetWeapon();
			const FNiagaraVariationData NiagaraVariationData(ProjectileData.bIsPlayerProjectile ? Weapon->GetPlayerData()->GetNiagaraVariationData(ProjectileData.GetProprietor()->GetVariation()) : Weapon->GetAIData()->GetNiagaraVariationData(EVariation::Default));
			Attributes.Add(TEXT("Color"), FNiagaraAttribute(true, NiagaraVariationData.Color));
			AssetsManager->AttachVfx(AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, EVfxType::Trail), nullptr, FNiagaraVariationData(), &BulletTrailComponent, Mesh, FName("BulletTrailSocket"), false, Attributes);
		}
		bHasBeenPooled = true;
	}
}

void AProjectile::SetDirection(const FVector InDirection) const { MovementComponent->Velocity = InDirection * MovementComponent->MaxSpeed; }

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
	Mesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::PlaySound(const ESound InSoundName) { Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : AssetsManager->ConfigureSound(Sounds[InSoundName], nullptr, InSoundName); }

void AProjectile::OnImpact(const FHitResult& Hit)
{
	DestroyThis();
	SpawnFirstProjectileImpactVfx(Hit);
	SpawnSecondProjectileImpactVfx(Hit);
	PlayImpactSound();
}

void AProjectile::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale, const int32 InWhich)
{
	Super::Init(InProjectileData, Rotation, Scale, InWhich);
	SetupProjectile(InProjectileData, Scale, InWhich);
	SetDirection(FMath::VRandCone(Rotation.Vector(), FMath::DegreesToRadians(ProjectileData.GetCalcSpread())));
	if (BulletTrailComponent) BulletTrailComponent->Activate();
	MovementComponent->Activate();
}

void AProjectile::DisableProjectileComponents() const
{
	if (BulletTrailComponent) BulletTrailComponent->Deactivate();
	MovementComponent->Deactivate();
}

void AProjectile::SpawnProjectileImpactVfx(const FHitResult& InHit, const bool bFirstImpact)
{
	const TObjectPtr<AWeapon> Weapon = ProjectileData.GetProprietor()->GetWeapon();
	const FNiagaraVariationData NiagaraVariationData(ProjectileData.bIsPlayerProjectile ? Weapon->GetPlayerData()->GetNiagaraVariationData(ProjectileData.GetProprietor()->GetVariation()) : Weapon->GetAIData()->GetNiagaraVariationData(EVariation::Default));
	Attributes.Add(TEXT("Color"), FNiagaraAttribute(true, NiagaraVariationData.Color));
	AssetsManager->SpawnVfx(AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, bFirstImpact ? EVfxType::Impact : EVfxType::SecondaryImpact), nullptr, InHit.ImpactPoint, InHit.ImpactNormal.Rotation(), NiagaraVariationData, Attributes);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this || OtherActor == nullptr) return;
	OnImpact(Hit);
	if (ProjectileData.bProjectileDealsAreaDamage)
	{
		if (ProjectileData.ProjectileName == EPoolObject::RangerGrenadeLauncherProjectile)
		DealAreaDamage();
		return;
	}
	if (OtherActor->ActorHasTag("Destructible"))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, Hit.ImpactNormal, Hit, nullptr, this, UDamageType::StaticClass());
		return;
	}
	if (!OtherActor->ActorHasTag("Enemy")) return;
	if (Mesh->GetCollisionObjectType() == ECC_GameTraceChannel1) // If enemy got hit.
	{
		DealDamageToSentientActor(OtherActor, Hit);
		return;
	}
	if (Mesh->GetCollisionObjectType() == ECC_GameTraceChannel2 && Cast<AEnemy>(OtherActor)->GetIsPossessed()) DealDamageToSentientActor(OtherActor, Hit); // If player got hit.
}

EDamageReason AProjectile::GetDamageReason() const { return ProjectileData.DamageReason; }

void AProjectile::AI_SetupUpgradesAppliance(const bool bIsPlayerProjectile)
{
	Damage += GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayerProjectile, ProjectileData.ProjectileName, EUpgradeAttribute::FlatDamage);
	const float DamagePercentage = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayerProjectile, ProjectileData.ProjectileName, EUpgradeAttribute::DamagePercentage);
	if (DamagePercentage > 0.f) Damage += DefaultDamage * (DamagePercentage / 100.f);
}

void AProjectile::P_SetupLevelUpAppliance(const int32 InLevel)
{
	Damage -= DefaultDamage * ProjectileData.GetProprietor()->GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease(InLevel - 1) * 0.01f;
	Damage += DefaultDamage * ProjectileData.GetProprietor()->GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease(InLevel) * 0.01f;
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	Super::EndPlay(EndPlayReason);
}

void AProjectile::SetupData(const bool bIsPlayerProjectile, const int32 InWhich)
{
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &AProjectile::DestroyThis, ProjectileData.LifeSpan, false);
	MovementComponent->InitialSpeed = ProjectileData.GetMultiProjectileData(ProjectileData.Amount).Data[InWhich].InitialSpeed;
	MovementComponent->MaxSpeed = ProjectileData.GetMultiProjectileData(ProjectileData.Amount).Data[InWhich].MaxSpeed;
	MovementComponent->ProjectileGravityScale = ProjectileData.GetMultiProjectileData(ProjectileData.Amount).Data[InWhich].GravityScale;
	Damage = DefaultDamage = ProjectileData.Damage;
	if (bIsPlayerProjectile)
	{
		ApplyCurrentVariation();
		Damage += DefaultDamage * ProjectileData.GetProprietor()->GetGameInstance()->GetSaveSlot()->GetLevelDamageIncrease() * 0.01f;
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnLevelUp.AddUObject(this, &AProjectile::P_SetupLevelUpAppliance);
	}
	else if (ProjectileData.GetProprietor()->GetIsElite()) Damage += DefaultDamage * 0.01f * ProjectileData.GetProprietor()->GetAIData()->GetEliteDamageIncreasePercentage();
	else if (ProjectileData.GetProprietor()->GetIsMutant()) Damage -= DefaultDamage * 0.01f * ProjectileData.GetProprietor()->GetAIData()->GetMutantDamageIncreasePercentage();
	AI_SetupUpgradesAppliance(bIsPlayerProjectile);
}

void AProjectile::DealAreaDamage()
{
	TArray<TObjectPtr<AActor>> IgnoredActors;
	if (!ProjectileData.bCanBeDamagedByOwnProjectile)
	{
		if (!ProjectileData.bIsPlayerProjectile) IgnoredActors = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetAllEnemies();
		else IgnoredActors = { ProjectileData.GetProprietor() };
	}
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), Damage, ProjectileData.RadiusMinimumDamage,
	GetActorLocation(), ProjectileData.DamageInnerRadius, ProjectileData.DamageOuterRadius, ProjectileData.DamageFalloff, UDamageType::StaticClass(),
	IgnoredActors, this, nullptr, ECC_GameTraceChannel4);
//	DrawDebugSphere(GetWorld(), GetActorLocation(), 100.f, 12, FColor::Green, false, 3.f);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 500.f, 12, FColor::Red, false, 3.f);
}

void AProjectile::DealDamageToSentientActor(const TObjectPtr<AActor>& OtherActor, const FHitResult& Hit)
{
	const float DamageMultiplier = Hit.BoneName == "head" ? 2.f : 1.f;
	UGameplayStatics::ApplyPointDamage(OtherActor, Damage * DamageMultiplier, Hit.ImpactNormal, Hit, nullptr, this, UDamageType::StaticClass());
}

void AProjectile::SpawnFirstProjectileImpactVfx(const FHitResult& Hit) { SpawnProjectileImpactVfx(Hit, true); }

void AProjectile::ApplyCurrentVariation()
{
	if (ProjectileData.ProjectileName == EPoolObject::TricksterPoisonProjectile && EVariation::TricksterVariationPurple == GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(ProjectileData.GetProprietor()->GetCharacterName()))
	{
		Damage = DefaultDamage = DefaultDamage * 2.5f;
	}
}

void AProjectile::DestroyThis()
{
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	Attributes.Empty();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnLevelUp.RemoveAll(this);
	DisableProjectileComponents();
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(ProjectileData.ProjectileName, this);
}
