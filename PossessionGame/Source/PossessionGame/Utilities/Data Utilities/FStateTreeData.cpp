#include "FStateTreeData.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponent.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"

void FStateTreeData::SetEQSPointLocation(const FVector InValue)
{
	EQSTarget->SetActorLocation(InValue);
	bIsEQSPointAvailable = true;
}

void FStateTreeData::TriggerEvent(const EStateTreeEvent InEventName) const
{
	(void)OnStateTreeEvent.ExecuteIfBound(InEventName);
	switch (InEventName)
	{
		case EStateTreeEvent::RangerLaunchGrenade: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.Ranger.LaunchGrenade")); break;
		case EStateTreeEvent::MinionBossStomp: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.MinionBoss.Stomp")); break;
		case EStateTreeEvent::MinionBossLaserEye: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.MinionBoss.LaserEye"));
	}
}
