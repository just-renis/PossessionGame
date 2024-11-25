#include "STConditionCanUseAbility.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

bool USTConditionCanUseAbility::TestCondition(FStateTreeExecutionContext& Context) const { return Cast<AEnemyAIController>(Context.GetOwner())->CanExecuteAbility(AbilityIndex); }
