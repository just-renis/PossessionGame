#include "STConditionDoesPathExist.h"
#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"

bool USTConditionDoesPathExist::TestCondition(FStateTreeExecutionContext& Context) const
{
	TObjectPtr<AEnemyAIController> Controller = Cast<AEnemyAIController>(Context.GetOwner());
	TObjectPtr<UNavigationSystemV1> NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Context.GetWorld());
	TObjectPtr<ANavigationData> NavData = NavSys->GetNavDataForProps(Controller->GetNavAgentPropertiesRef(), Controller->GetNavAgentLocation());
	EPathFindingMode::Type TestMode = PathQueryType == EPathFindingType::HierarchicalQuery ? EPathFindingMode::Hierarchical : EPathFindingMode::Regular;
	if (Target)
	{
		return bInvert ? !NavSys->TestPathSync(FPathFindingQuery(Controller, *NavData, Controller->GetPawn()->GetActorLocation(), Target->GetActorLocation()), TestMode) :
		NavSys->TestPathSync(FPathFindingQuery(Controller, *NavData, Controller->GetPawn()->GetActorLocation(), Target->GetActorLocation()), TestMode);
	}
	return bInvert ? !NavSys->TestPathSync(FPathFindingQuery(Controller, *NavData, Controller->GetPawn()->GetActorLocation(), TargetLocation), TestMode) :
	NavSys->TestPathSync(FPathFindingQuery(Controller, *NavData, Controller->GetPawn()->GetActorLocation(), TargetLocation), TestMode);
}
