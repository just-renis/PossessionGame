#include "STTaskWraithMeteorShower.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/WraithAIController.h"

EStateTreeRunStatus USTTaskWraithMeteorShower::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<AWraithAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::WraithMeteorShower);
	CachedController->GetProprietor()->AI_UseAbility(2);
	CachedController->OnMeteorShowerFinished.BindUObject(this, &USTTaskWraithMeteorShower::MeteorShowerFinished);
	return Super::EnterState(Context, Transition);
}

void USTTaskWraithMeteorShower::MeteorShowerFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
