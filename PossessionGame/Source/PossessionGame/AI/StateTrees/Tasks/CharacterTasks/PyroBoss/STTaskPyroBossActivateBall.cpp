#include "STTaskPyroBossActivateBall.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Controllers/PyroBossAIController.h"

EStateTreeRunStatus USTTaskPyroBossActivateBall::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<APyroBossAIController>(Context.GetOwner());
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::PyroBossBallPhase);
	CachedController->GetProprietor()->AI_UseAbility(2);
	CachedController->OnBallPhaseFinished.BindUObject(this, &USTTaskPyroBossActivateBall::BallPhaseFinished);
	return Super::EnterState(Context, Transition);
}

void USTTaskPyroBossActivateBall::BallPhaseFinished()
{
	CachedController->SetStateTreeCurrentEvent(EStateTreeEvent::None);
	FinishTask();
}
