#include "STTaskUseAbility.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

EStateTreeRunStatus USTTaskUseAbility::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Cast<AEnemyAIController>(Context.GetOwner())->GetProprietor()->AI_UseAbility(AbilitySlot);
	return EStateTreeRunStatus::Succeeded;
}
