#include "MeteorShowerArea.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/DamageOverTimeData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void AMeteorShowerArea::BeginPlay()
{
	Super::BeginPlay();
	ObjectName = EPoolObject::WraithMeteorShowerArea;
}

void AMeteorShowerArea::DealOvertimeDamage() { SpawnMeteor(); }

void AMeteorShowerArea::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	Proprietor = InProprietor;
}

void AMeteorShowerArea::Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason)
{
	bIsHijacked = bInIsPlayerObject;
	Super::Setup(bInIsPlayerObject, InDamageOverTimeData, InDamageReason);
	const float LifeSpanIncrease = 1.f + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility3, EUpgradeAttribute::EffectDuration, bInIsPlayerObject) * 0.01f;
	DamageTick = DefaultDamageTick - GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility3, EUpgradeAttribute::OverTimeTick, bInIsPlayerObject);
	SetupRadiusAndTimers(DefaultRadius, InDamageOverTimeData->GetAreaLifeSpan() * LifeSpanIncrease);
	if (!DeployVfx)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::WraithTriggerMeteorImpact, nullptr, &DeployVfx, RootComponent, NAME_None,
		false, FVector::ZeroVector, FRotator(-90.f, 0.f, 0.f));
	}
	else DeployVfx->Activate(true);
}

void AMeteorShowerArea::SpawnMeteor()
{
	const FVector Center = GetCollisionComponent()->GetComponentLocation();
	FVector2D RandDir2D = FVector2D(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f));
	RandDir2D.Normalize();
	const float Distance = FMath::FRandRange(0.f, GetCollisionComponent()->GetScaledSphereRadius() - MeteorSpawnRadiusReduction);
	const FVector RandomLocation(Center.X + RandDir2D.X * Distance, Center.Y + RandDir2D.Y * Distance, Center.Z);
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithMeteorProjectile, RandomLocation)->InitWithProprietor(Proprietor);
}
