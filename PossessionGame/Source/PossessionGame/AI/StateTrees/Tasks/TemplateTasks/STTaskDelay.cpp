#include "STTaskDelay.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/KismetSystemLibrary.h"

EStateTreeRunStatus USTTaskDelay::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (!bUseCustomDelay && !bUseUnlimitedDelay) UKismetSystemLibrary::K2_SetTimer(this, "OnDelayCompleted", Delay, false);
	return Super::EnterState(Context, Transition);
}

void USTTaskDelay::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UKismetSystemLibrary::K2_ClearTimer(this, "OnDelayCompleted");
	Super::ExitState(Context, Transition);
}

void USTTaskDelay::OnDelayCompleted()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "OnDelayCompleted");
	FinishTask();
}