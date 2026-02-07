#include "STTaskPyroBossActivateFireFloor.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/PyroBossAIController.h"

EStateTreeRunStatus USTTaskPyroBossActivateFireFloor::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<APyroBossAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::PyroBossActivateFireFloor);
	CachedController->GetProprietor()->AI_UseAbility(0);
	CachedController->OnActivateFireFloorFinished.BindUObject(this, &USTTaskPyroBossActivateFireFloor::ActivateFireFloorFinished);
	return Super::EnterState(Context, Transition);
}

void USTTaskPyroBossActivateFireFloor::ActivateFireFloorFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
