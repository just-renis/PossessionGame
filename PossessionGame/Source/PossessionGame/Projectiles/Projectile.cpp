#include "Projectile.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/AI/Base Data/AICharacterData.h"
#include "PossessionGame/Enums/EUpgrade.h"
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
	MovementComponent->SetComponentTickInterval(1.f / 30.f);
	MovementComponent->bAutoRegisterPhysicsVolumeUpdates = false;
	MovementComponent->bComponentShouldUpdatePhysicsVolume = false;
	MovementComponent->UpdatedComponent = RootComponent;
}

void AProjectile::SetupProjectile(const FProjectileData& InProjectileData, const int32 InWhich)
{
	StartLocation = GetActorLocation();
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
	else AssetsManager->ChangeVfxVariation(AssetsManager->GetVfxData(AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, EVfxType::Trail)), nullptr, BulletTrailComponent, false, ProjectileData.GetProprietor()->GetVariation()); // FIX THIS CRAP LATER.
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

void AProjectile::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich)
{
	Super::Init(InProjectileData, Rotation, InWhich);
	SetupProjectile(InProjectileData, InWhich);
	SetDirection(FMath::VRandCone(Rotation.Vector(), FMath::DegreesToRadians(ProjectileData.GetCalcSpread())));
	if (BulletTrailComponent) BulletTrailComponent->Activate();
	MovementComponent->SetComponentTickEnabled(true);
	MovementComponent->Activate();
}

void AProjectile::InitCloneProjectile(FProjectileData& InProjectileData, const FRotator& Rotation)
{
	Super::InitCloneProjectile(InProjectileData, Rotation);
	StartLocation = GetActorLocation();
	ProjectileData = InProjectileData;
	ProjectileData.ProjectileName = EPoolObject::CloneProjectile;
	Mesh->SetCollisionObjectType(ProjectileData.bIsPlayerProjectile ? ECC_GameTraceChannel1 : ECC_GameTraceChannel2);
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &AProjectile::DestroyThis, ProjectileData.LifeSpan, false);
	const FAdditionalProjectileData AdditionalProjectileData = ProjectileData.GetMultiProjectileData(0).Data[0];
	MovementComponent->InitialSpeed = AdditionalProjectileData.InitialSpeed;
	MovementComponent->MaxSpeed = AdditionalProjectileData.MaxSpeed;
	MovementComponent->ProjectileGravityScale = AdditionalProjectileData.GravityScale;
	Damage = DefaultDamage = ProjectileData.Damage;
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	if (RunSaveSlot->IsSpecialUpgradeAvailable(EUpgrade::TricksterAbility3, EUpgradeAttribute::Special1))
	{
		Damage += DefaultDamage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease() * 0.01f;
	}
	Damage += RunSaveSlot->GetUpgradeAttributeValues(ProjectileData.ProjectileName, EUpgradeAttribute::FlatDamage, true);
	if (!bHasBeenPooled)
	{
		AssetsManager->AttachVfx(EVfx::CloneBullet, nullptr, &BulletTrailComponent, Mesh, FName("BulletTrailSocket"));
		bHasBeenPooled = true;
	}
	SetDirection(Rotation.Vector());
	if (BulletTrailComponent) BulletTrailComponent->Activate();
	MovementComponent->SetComponentTickEnabled(true);
	MovementComponent->Activate();
}

void AProjectile::DisableProjectileComponents() const
{
	if (BulletTrailComponent) BulletTrailComponent->Deactivate();
	MovementComponent->SetComponentTickEnabled(false);
	MovementComponent->Deactivate();
}

void AProjectile::SpawnProjectileImpactVfx(const FHitResult& InHit, const bool bFirstImpact)
{
	AssetsManager->SpawnVfx(AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, bFirstImpact ? EVfxType::Impact : EVfxType::SecondaryImpact), nullptr,
	InHit.ImpactPoint, InHit.ImpactNormal.Rotation(), ProjectileData.GetProprietor() ? ProjectileData.GetProprietor()->GetVariation() : EVariation::Default, Attributes);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this || OtherActor == nullptr) return;
	OnImpact(Hit);
	if (ProjectileData.bProjectileDealsAreaDamage)
	{
		DealAreaDamage();
		return;
	}
	if (!ProjectileData.bIsProjectileDealingDamageItself) return;
	if (OtherActor->ActorHasTag("Destructible"))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, (Hit.Location - GetActorLocation()).GetSafeNormal() * 2.f, Hit, ProjectileData.GetProprietor() ? ProjectileData.GetProprietor()->GetController() : nullptr, this, UDamageType::StaticClass());
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
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	Damage += RunSaveSlot->GetUpgradeAttributeValues(ProjectileData.ProjectileName, EUpgradeAttribute::FlatDamage, false);
	const float DamagePercentage = RunSaveSlot->GetUpgradeAttributeValues(ProjectileData.ProjectileName, EUpgradeAttribute::DamagePercentage, false);
	Damage += DefaultDamage * DamagePercentage * 0.01f;
	Damage += AI_UpgradeDamageAppliance();
}

void AProjectile::P_SetupLevelUpAppliance(const int32 InLevel)
{
	Damage -= DefaultDamage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease(InLevel - 1) * 0.01f;
	Damage += DefaultDamage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease(InLevel) * 0.01f;
}

void AProjectile::P_SetupUpgradesAppliance()
{
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	Damage += RunSaveSlot->GetUpgradeAttributeValues(ProjectileData.ProjectileName, EUpgradeAttribute::FlatDamage, true);
	const float DamagePercentage = RunSaveSlot->GetUpgradeAttributeValues(ProjectileData.ProjectileName, EUpgradeAttribute::DamagePercentage, true);
	Damage += DefaultDamage * DamagePercentage * 0.01f;
	Damage += P_UpgradeDamageAppliance();
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
		Damage += DefaultDamage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardDamage) * 0.01f;
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
	IgnoredActors, this, ProjectileData.GetProprietor()->GetController(), ECC_GameTraceChannel4);
//	DrawDebugSphere(GetWorld(), GetActorLocation(), 100.f, 12, FColor::Green, false, 3.f);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 500.f, 12, FColor::Red, false, 3.f);
}

void AProjectile::DealDamageToSentientActor(const bool bIsPlayerProjectile, const TObjectPtr<AActor>& OtherActor, const FHitResult& Hit)
{
	if (!bIsPlayerProjectile)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, (Hit.Location - GetActorLocation()).GetSafeNormal() * 2.f, Hit, ProjectileData.GetProprietor()->GetController(), this, UDamageType::StaticClass());
		return;
	}
	const bool bIsCritical = Hit.BoneName.IsEqual(FName("head"));
	const float DamageAfterCritical = ProjectileData.GetProprietor() ? Damage * (bIsCritical ? ProjectileData.GetProprietor()->GetWeapon()->GetPlayerData()->GetCriticalDamage() : 1.f) : Damage;
	FHitResult ModifiableHit = Hit;
	ModifiableHit.TraceStart = StartLocation;
	UGameplayStatics::ApplyPointDamage(OtherActor, DamageAfterCritical, (ModifiableHit.Location - GetActorLocation()).GetSafeNormal() * 2.f, ModifiableHit, ProjectileData.GetProprietor() ? ProjectileData.GetProprietor()->GetController() : nullptr, this, UDamageType::StaticClass());
	if (ProjectileData.GetProprietor()) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnPlayerDamageDealt.ExecuteIfBound(bIsCritical);
}

void AProjectile::SpawnFirstProjectileImpactVfx(const FHitResult& Hit) { SpawnProjectileImpactVfx(Hit, true); }

void AProjectile::DestroyThis()
{
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnLevelUp.RemoveAll(this);
	DisableProjectileComponents();
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(ProjectileData.ProjectileName, this);
}
