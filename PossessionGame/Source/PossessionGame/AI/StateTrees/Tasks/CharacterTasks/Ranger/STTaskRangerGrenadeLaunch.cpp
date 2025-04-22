#include "STTaskRangerGrenadeLaunch.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/RangerAIController.h"

EStateTreeRunStatus USTTaskRangerGrenadeLaunch::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	const TObjectPtr<ARangerAIController> CachedController = Cast<ARangerAIController>(Context.GetOwner());
	CachedController->GetProprietor()->AI_UseAbility(0);
	CachedController->OnGrenadeLaunchFinished.BindUObject(this, &USTTaskRangerGrenadeLaunch::GrenadeLaunchFinished);
	return EStateTreeRunStatus::Running;
}

void USTTaskRangerGrenadeLaunch::GrenadeLaunchFinished() { FinishTask(); }
