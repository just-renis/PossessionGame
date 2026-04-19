#include "STTaskPyroBossStomp.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/PyroBossAIController.h"

EStateTreeRunStatus USTTaskPyroBossStomp::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<APyroBossAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::PyroBossStomp);
	CachedController->GetProprietor()->AI_UseAbility(1);
	CachedController->OnStompFinished.BindUObject(this, &USTTaskPyroBossStomp::StompFinished);
	return Super::EnterState(Context, Transition);
}

void USTTaskPyroBossStomp::StompFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
