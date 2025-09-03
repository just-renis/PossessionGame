#include "STTaskTricksterLaunchPoison.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/TricksterAIController.h"

EStateTreeRunStatus USTTaskTricksterLaunchPoison::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<ATricksterAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::TricksterLaunchPoisonGrenade);
	CachedController->GetProprietor()->AI_UseAbility(0);
	CachedController->OnPoisonGrenadeLaunched.BindUObject(this, &USTTaskTricksterLaunchPoison::PoisonGrenadeLaunchFinished);
	return Super::EnterState(Context, Transition);
}

void USTTaskTricksterLaunchPoison::PoisonGrenadeLaunchFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
