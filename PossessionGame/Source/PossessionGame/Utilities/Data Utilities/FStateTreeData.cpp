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
		case EStateTreeEvent::RangerDeployShield: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.Ranger.DeployShield")); break;
		case EStateTreeEvent::RangerLaunchGrenade: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.Ranger.LaunchGrenade")); break;
		case EStateTreeEvent::TricksterLaunchPoisonGrenade: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.Trickster.LaunchPoisonGrenade")); break;
		case EStateTreeEvent::MinionBossStomp: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.MinionBoss.Stomp")); break;
		case EStateTreeEvent::MinionBossLaserEye: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.MinionBoss.LaserEye")); break;
		case EStateTreeEvent::MinionBossMoveDirectlyToPlayer: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.MinionBoss.MoveDirectlyToPlayer")); break;
		case EStateTreeEvent::WardenBeamShot: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.Warden.BeamShot")); break;
		case EStateTreeEvent::BreacherBash: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.Breacher.Bash")); break;
		case EStateTreeEvent::WraithMeteorShower: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.Wraith.MeteorShower")); break;
		case EStateTreeEvent::PyroBossActivateFireFloor: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.PyroBoss.ActivateFireFloor")); break;
		case EStateTreeEvent::PyroBossStomp: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.PyroBoss.Stomp")); break;
		case EStateTreeEvent::MoveDirectly: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.MoveDirectly")); break;
		case EStateTreeEvent::MoveWithPathfinder: StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.MoveWithPathfinder")); break;
		default: ;
	}
}
