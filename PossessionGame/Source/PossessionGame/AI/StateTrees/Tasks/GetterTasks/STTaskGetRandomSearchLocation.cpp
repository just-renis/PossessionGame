#include "STTaskGetRandomSearchLocation.h"
#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

EStateTreeRunStatus USTTaskGetRandomSearchLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	TObjectPtr<AEnemyAIController> Controller = Cast<AEnemyAIController>(Context.GetOwner());
	FNavLocation Location;
	if (Controller->GetStateTreeData().GetTarget())
	{
		UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomReachablePointInRadius(Controller->GetStateTreeData().GetTargetLocation(), SearchRadius, Location);
		Controller->GetStateTreeData().SetSearchLocation(Location.Location);
	}
	return EStateTreeRunStatus::Succeeded;
}
