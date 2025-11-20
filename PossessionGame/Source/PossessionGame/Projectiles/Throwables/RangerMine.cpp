#include "RangerMine.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void ARangerMine::Disable(const bool bHide)
{
	DestroyThis();
	Super::Disable(bHide);
}

void ARangerMine::DestroyThis() const
{
	if (ImpactVfx) ImpactVfx->DeactivateImmediate();
	if (RadiusVfx) RadiusVfx->DeactivateImmediate();
	MineTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

ARangerMine::ARangerMine()
{
	MineTriggerCollision = CreateDefaultSubobject<USphereComponent>("MineTriggerCollision");
	MineTriggerCollision->SetupAttachment(Mesh);
	MineTriggerCollision->SetCollisionProfileName(FName("TriggerPlayer"));
	MineTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MineTriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ARangerMine::TriggeredCollision);
	DeploySfx = CreateDefaultSubobject<UAudioComponent>(TEXT("MineImpactSfx"));
	DeploySfx->SetAutoActivate(false);
	DeploySfx->SetupAttachment(Mesh);
	ExplosionSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("MineExplosionSfx"));
	ExplosionSfx->SetAutoActivate(false);
	ExplosionSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::RangerDeployMine, DeploySfx);
	Sounds.Add(ESound::RangerDetonateMine, ExplosionSfx);
}

float ARangerMine::UpgradeAppliance() const
{
	return Data->GetDamage() * GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetRangerGrenadeSpecialBoost()
	+ Data->GetDamage() * GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetRangerShieldSpecialBoost();
}

void ARangerMine::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSpecialUpgradeAdded.AddUObject(this, &ARangerMine::ChangeRadiusVfx);
	if (GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::RangerAbility3, EUpgradeAttribute::Special1))
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerMineSpecialRadius, [this]() -> bool { return bAlreadyLanded; }, &ImpactVfx, Mesh);
	}
	else GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerMineImpact, [this]() -> bool { return bAlreadyLanded; }, &ImpactVfx, Mesh);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerMineRadius, [this]() -> bool { return bAlreadyLanded; }, &RadiusVfx, Mesh);
}

void ARangerMine::OnImpact(const FHitResult& Hit)
{
	Super::OnImpact(Hit);
	bAlreadyLanded = true;
	PlaySound(ESound::RangerDeployMine);
	if (ImpactVfx) ImpactVfx->Activate();
	if (RadiusVfx)
	{
		TMap<FName, FNiagaraAttribute> Attributes;
		const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::RangerAbility3, EUpgradeAttribute::Size, true);
		const float CollisionRadius = DefaultCollisionRadius + DefaultCollisionRadius * RadiusIncrease * 0.01f;
		Attributes.Add(FName("Mesh Uniform Scale"), FNiagaraAttribute(true, CollisionRadius / DefaultCollisionRadius));
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->UpdateVfxAttributes(RadiusVfx, Attributes);
		MineTriggerCollision->SetSphereRadius(CollisionRadius);
		RadiusVfx->Activate();
	}
	MineTriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ARangerMine::DealAreaDamage()
{
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	const bool bIsSpecialUpgradeAvailable1 = RunSaveSlot->IsSpecialUpgradeAvailable(EUpgrade::RangerAbility3, EUpgradeAttribute::Special1);
	const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::RangerAbility3, EUpgradeAttribute::Size, true);
	const float DamageIncrease = Data->GetDamage() * RunSaveSlot->GetUpgradeAttributeValues(EUpgrade::RangerAbility3, EUpgradeAttribute::DamagePercentage, true) * 0.01f;
	float NormalDamage = (Data->GetDamage() + DamageIncrease) * (bIsSpecialUpgradeAvailable1 ? SpecialUpgradeDamageMultiplier : 1.f);
	float MinimumDamage = Data->GetRadiusMinimumDamage() + DamageIncrease;
	const float InnerRadius = Data->GetDamageInnerRadius() + Data->GetDamageInnerRadius() * RadiusIncrease * 0.01f;
	const float OuterRadius = Data->GetDamageOuterRadius() + Data->GetDamageOuterRadius() * RadiusIncrease * 0.01f;
	const float LevelDamageBoost = Data->GetDamage() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetLevelDamageIncrease() * 0.01f;
	NormalDamage += LevelDamageBoost;
	MinimumDamage += LevelDamageBoost;
	const float UpgradeDamageBoost = UpgradeAppliance();
	NormalDamage += UpgradeDamageBoost;
	MinimumDamage += UpgradeDamageBoost;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), NormalDamage, MinimumDamage, GetActorLocation(), InnerRadius, OuterRadius,
	Data->GetDamageFalloff(), UDamageType::StaticClass(), TArray{ Proprietor }, this, nullptr, ECC_GameTraceChannel4);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::RangerMineExplosion, nullptr, GetActorLocation());
}

void ARangerMine::Explode()
{
	DealAreaDamage();
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestObjectWithCapRemoval.ExecuteIfBound(EPoolObject::RangerMine, this);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::RangerMine, this);
	bInExplosionProcess = false;
}

void ARangerMine::ChangeRadiusVfx(const EUpgrade InUpgradeName, const EUpgradeAttribute InSpecialUpgradeAttributeName)
{
	if (RadiusVfx)
	{
		RadiusVfx->DestroyComponent();
		RadiusVfx = nullptr;
	}
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerMineSpecialRadius, [this]() -> bool { return bAlreadyLanded; }, &RadiusVfx, Mesh);
}

void ARangerMine::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (FMath::IsNearlyEqual(Hit.ImpactNormal.Z, 1.f, 0.1f)) OnImpact(Hit);
}

void ARangerMine::TriggeredCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag(FName("AliveEnemy"))) return;
	bInExplosionProcess = true;
	PlaySound(ESound::RangerDetonateMine);
	DestroyThis();
	GetWorldTimerManager().SetTimer(ExplosionHandle, this, &ARangerMine::Explode, ExplosionDelay);
}
