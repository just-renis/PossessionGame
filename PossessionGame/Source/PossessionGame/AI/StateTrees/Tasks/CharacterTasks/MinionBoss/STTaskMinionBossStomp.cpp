#include "STTaskMinionBossStomp.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/MinionBossAIController.h"

EStateTreeRunStatus USTTaskMinionBossStomp::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	const TObjectPtr<AMinionBossAIController> CachedController = Cast<AMinionBossAIController>(Context.GetOwner());
	CachedController->GetProprietor()->AI_UseAbility(0);
	CachedController->OnStompFinished.BindUObject(this, &USTTaskMinionBossStomp::StompFinished);
	return EStateTreeRunStatus::Running;
}

void USTTaskMinionBossStomp::StompFinished() { FinishTask(); }
