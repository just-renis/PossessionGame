#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/StateTrees/Tasks/TemplateTasks/STTaskMoveToActor.h"
#include "STTaskMoveToEQSLocation.generated.h"

UCLASS(DisplayName = "Move To EQS Location", Category = "Movement Tasks")
class POSSESSIONGAME_API USTTaskMoveToEQSLocation : public USTTaskMoveToActor
{
	GENERATED_BODY()

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
