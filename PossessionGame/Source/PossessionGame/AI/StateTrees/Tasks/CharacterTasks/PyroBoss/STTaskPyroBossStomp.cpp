#include "STTaskPyroBossStomp.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/PyroBossAIController.h"

EStateTreeRunStatus USTTaskPyroBossStomp::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<APyroBossAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::PyroBossActivateFireFloor);
	CachedController->GetProprietor()->AI_UseAbility(1);
	CachedController->OnActivateStompFinished.BindUObject(this, &USTTaskPyroBossStomp::ActivateStompFinished);
	return Super::EnterState(Context, Transition);
}

void USTTaskPyroBossStomp::ActivateStompFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
