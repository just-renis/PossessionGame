#include "PyroBossProjectileFirePool.h"
#include "NiagaraComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DamageOverTimeData.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void APyroBossProjectileFirePool::BeginPlay()
{
	Super::BeginPlay();
	ObjectName = EPoolObject::PyroBossProjectileFirePool;
}

void APyroBossProjectileFirePool::Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason)
{
	Super::Setup(bInIsPlayerObject, InDamageOverTimeData, InDamageReason);
	SetupRadiusAndTimers(InDamageOverTimeData->GetAreaSize(), InDamageOverTimeData->GetAreaLifeSpan());
	if (!RadiusVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::PyroBossProjectileFirePool, [this]() -> bool { return bIsActive; }, &RadiusVfx, RootComponent);
	else RadiusVfx->Activate(true);
}

void APyroBossProjectileFirePool::DealOvertimeDamage()
{
	if (Targets.IsEmpty()) return;
	for (int32 i = 0; i < Targets.Num(); i++)
	{
		if (!Targets[i]) continue;
		Targets[i]->SetupBurningState(EBurningType::PyroBossProjectileFirePool, Damage, EffectTick, DamageTick);
	}
}

void APyroBossProjectileFirePool::Disable(const bool bHide)
{
	Super::Disable(bHide);
	if (!RadiusVfx) return;
	RadiusVfx->DeactivateImmediate();
}
