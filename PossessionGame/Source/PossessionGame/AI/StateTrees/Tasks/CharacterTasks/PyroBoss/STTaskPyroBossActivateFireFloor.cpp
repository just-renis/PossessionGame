#include "STTaskPyroBossActivateFireFloor.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/PyroBossAIController.h"

EStateTreeRunStatus USTTaskPyroBossActivateFireFloor::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<APyroBossAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::PyroBossActivateFireFloor);
	CachedController->GetProprietor()->AI_UseAbility(0);
	CachedController->OnFireFloorFinished.BindUObject(this, &USTTaskPyroBossActivateFireFloor::FireFloorFinished);
	return Super::EnterState(Context, Transition);
}

void USTTaskPyroBossActivateFireFloor::FireFloorFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
