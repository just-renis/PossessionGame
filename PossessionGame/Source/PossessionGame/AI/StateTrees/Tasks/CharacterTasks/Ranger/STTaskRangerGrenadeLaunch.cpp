#include "STTaskRangerGrenadeLaunch.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/RangerAIController.h"

EStateTreeRunStatus USTTaskRangerGrenadeLaunch::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<ARangerAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::RangerLaunchGrenade);
	CachedController->GetProprietor()->AI_UseAbility(0);
	CachedController->OnGrenadeLaunchFinished.BindUObject(this, &USTTaskRangerGrenadeLaunch::GrenadeLaunchFinished);
	return Super::EnterState(Context, Transition);
}

void USTTaskRangerGrenadeLaunch::GrenadeLaunchFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
