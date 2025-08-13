#include "Projectile.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/AI/Base Data/AICharacterData.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = Mesh;
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("movementComponent"));
	MovementComponent->InitialSpeed = 2000.f;
	MovementComponent->MaxSpeed = 2000.f;
	MovementComponent->ProjectileGravityScale = 0.f;
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
			AssetsManager->AttachVfx(AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, EVfxType::Trail), nullptr, &BulletTrailComponent, Mesh, FName("BulletTrailSocket"), false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, ProjectileData.GetProprietor()->GetVariation(), Attributes);
		}
		bHasBeenPooled = true;
	}
	else AssetsManager->ChangeVfxVariation(AssetsManager->GetVfxData(AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, EVfxType::Trail)), nullptr, BulletTrailComponent, false, ProjectileData.GetProprietor()->GetVariation());
}

void AProjectile::SetDirection(const FVector& InDirection) const { MovementComponent->Velocity = InDirection * MovementComponent->MaxSpeed; }

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
	UpgradeManager = GetWorld()->GetSubsystem<UUpgradeManager>();
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
	AssetsManager->SpawnVfx(AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, bFirstImpact ? EVfxType::Impact : EVfxType::SecondaryImpact), nullptr, InHit.ImpactPoint, InHit.ImpactNormal.Rotation(), ProjectileData.GetProprietor()->GetVariation(), Attributes);
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
	const bool bIsPlayerProjectile = Mesh->GetCollisionObjectType() == ECC_GameTraceChannel1;
	if (bIsPlayerProjectile)
	{
		DealDamageToSentientActor(bIsPlayerProjectile, OtherActor, Hit);
		return;
	}
	const bool bIsEnemyProjectile = Mesh->GetCollisionObjectType() == ECC_GameTraceChannel2;
	if (bIsEnemyProjectile && Cast<AEnemy>(OtherActor)->GetIsHijacked()) DealDamageToSentientActor(bIsPlayerProjectile, OtherActor, Hit);
}

EDamageReason AProjectile::GetDamageReason() const { return ProjectileData.DamageReason; }

void AProjectile::AI_SetupUpgradesAppliance()
{
	Damage += GetWorld()->GetSubsystem<UUpgradeManager>()->AI_GetTotalUpgradeAttributeValue(EUpgradeAttribute::FlatDamage);
	const float DamagePercentage = UpgradeManager->GetUpgradeAttributeValue(false, ProjectileData.ProjectileName, EUpgradeAttribute::DamagePercentage);
	if (DamagePercentage > 0.f) Damage += DefaultDamage * DamagePercentage * 0.01f;
}

void AProjectile::P_SetupLevelUpAppliance(const int32 InLevel)
{
	Damage -= DefaultDamage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease(InLevel - 1) * 0.01f;
	Damage += DefaultDamage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease(InLevel) * 0.01f;
}

void AProjectile::P_SetupUpgradesAppliance()
{
	Damage += GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(true, ProjectileData.ProjectileName, EUpgradeAttribute::FlatDamage);
	const float DamagePercentage = UpgradeManager->GetUpgradeAttributeValue(true, ProjectileData.ProjectileName, EUpgradeAttribute::DamagePercentage);
	if (DamagePercentage > 0.f) Damage += DefaultDamage * DamagePercentage * 0.01f;
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	Super::EndPlay(EndPlayReason);
}

void AProjectile::SetupData(const bool bIsPlayerProjectile, const int32 InWhich)
{
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &AProjectile::DestroyThis, ProjectileData.LifeSpan, false);
	const FAdditionalProjectileData AdditionalProjectileData = ProjectileData.GetMultiProjectileData(ProjectileData.Amount).Data[InWhich];
	MovementComponent->InitialSpeed = AdditionalProjectileData.InitialSpeed;
	MovementComponent->MaxSpeed = AdditionalProjectileData.MaxSpeed;
	MovementComponent->ProjectileGravityScale = AdditionalProjectileData.GravityScale;
	Damage = DefaultDamage = ProjectileData.Damage;
	if (bIsPlayerProjectile)
	{
		P_SetupUpgradesAppliance();
		P_VariationDamageAppliance();
		Damage += DefaultDamage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease() * 0.01f;
		Damage += Damage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardDamage) * 0.01f;
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnLevelUp.AddUObject(this, &AProjectile::P_SetupLevelUpAppliance);
	}
	else
	{
		AI_SetupUpgradesAppliance();
		if (ProjectileData.GetProprietor()->GetIsMutant()) Damage -= Damage * 0.01f * ProjectileData.GetProprietor()->GetAIData()->GetMutantDamageIncreasePercentage();
	}
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

void AProjectile::DealDamageToSentientActor(const bool bIsPlayerProjectile, const TObjectPtr<AActor>& OtherActor, const FHitResult& Hit)
{
	if (!bIsPlayerProjectile)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, Hit.ImpactNormal, Hit, nullptr, this, UDamageType::StaticClass());
		return;
	}
	const bool bIsCritical = Hit.BoneName == "head";
	const float DamageMultiplier = bIsCritical ? ProjectileData.GetProprietor()->GetWeapon()->GetPlayerData()->GetCriticalDamage() : 1.f;
	UGameplayStatics::ApplyPointDamage(OtherActor, Damage * DamageMultiplier, Hit.ImpactNormal, Hit, nullptr, this, UDamageType::StaticClass());
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnPlayerDamageDealt.ExecuteIfBound(bIsCritical);
	AssetsManager->PlaySound2D(bIsCritical ? ESound::CriticalDamageTick : ESound::DamageTick);
}

void AProjectile::SpawnFirstProjectileImpactVfx(const FHitResult& Hit) { SpawnProjectileImpactVfx(Hit, true); }

void AProjectile::DestroyThis()
{
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnLevelUp.RemoveAll(this);
	DisableProjectileComponents();
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(ProjectileData.ProjectileName, this);
}
