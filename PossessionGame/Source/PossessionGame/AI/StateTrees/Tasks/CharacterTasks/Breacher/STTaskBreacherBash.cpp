#include "STTaskBreacherBash.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/BreacherAIController.h"

EStateTreeRunStatus USTTaskBreacherBash::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<ABreacherAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::BreacherBash);
	CachedController->GetProprietor()->AI_UseAbility(1);
	CachedController->OnBashFinished.BindUObject(this, &USTTaskBreacherBash::BashFinished);
	return Super::EnterState(Context, Transition);
}

void USTTaskBreacherBash::BashFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
