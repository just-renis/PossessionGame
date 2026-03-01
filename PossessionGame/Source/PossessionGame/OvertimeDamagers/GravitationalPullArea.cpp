#include "GravitationalPullArea.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DamageOverTimeData.h"
#include "PossessionGame/Enums/EPoolObject.h"

void AGravitationalPullArea::BeginPlay()
{
	Super::BeginPlay();
	ObjectName = EPoolObject::TankGravitationalPullArea;
}

void AGravitationalPullArea::DealOvertimeDamage()
{
	if (Targets.IsEmpty()) return;
	for (int32 i = 0; i < Targets.Num(); i++)
	{
		if (!Targets[i]) continue;
		Targets[i]->Pull(GetActorLocation(), EffectStrength);
	}
}

void AGravitationalPullArea::Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason)
{
	Super::Setup(bInIsPlayerObject, InDamageOverTimeData, InDamageReason);
	SetupRadiusAndTimers(InDamageOverTimeData->GetAreaSize(), InDamageOverTimeData->GetAreaLifeSpan());
}
