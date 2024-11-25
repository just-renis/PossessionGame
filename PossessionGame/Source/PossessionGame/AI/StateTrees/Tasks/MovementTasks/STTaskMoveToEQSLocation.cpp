#include "STTaskMoveToEQSLocation.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

void USTTaskMoveToEQSLocation::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController->GetStateTreeData().SetEQSPointAvailability(false);
	Super::ExitState(Context, Transition);
}
