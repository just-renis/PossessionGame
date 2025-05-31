#include "STTaskDelay.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus USTTaskDelay::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!bUseCustomDelay && !bUseUnlimitedDelay) GetWorld()->GetTimerManager().SetTimer(DelayHandle, this, &USTTaskDelay::OnDelayCompleted, Delay, false);
	return Super::EnterState(Context, Transition);
}

void USTTaskDelay::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	GetWorld()->GetTimerManager().ClearTimer(DelayHandle);
	Super::ExitState(Context, Transition);
}

void USTTaskDelay::OnDelayCompleted()
{
	GetWorld()->GetTimerManager().ClearTimer(DelayHandle);
	FinishTask();
}