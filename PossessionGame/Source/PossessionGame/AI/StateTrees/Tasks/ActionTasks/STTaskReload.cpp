#include "STTaskReload.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

EStateTreeRunStatus USTTaskReload::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Cast<AEnemyAIController>(Context.GetOwner())->GetProprietor()->AI_ReloadLogic();
	return EStateTreeRunStatus::Succeeded;
}