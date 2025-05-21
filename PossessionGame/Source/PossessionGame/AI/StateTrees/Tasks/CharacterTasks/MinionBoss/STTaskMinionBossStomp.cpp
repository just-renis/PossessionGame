#include "STTaskMinionBossStomp.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/MinionBossAIController.h"

EStateTreeRunStatus USTTaskMinionBossStomp::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<AMinionBossAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::MinionBossStomp);
	CachedController->GetProprietor()->AI_UseAbility(0);
	CachedController->OnStompFinished.BindUObject(this, &USTTaskMinionBossStomp::StompFinished);
	return EStateTreeRunStatus::Running;
}

void USTTaskMinionBossStomp::StompFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
