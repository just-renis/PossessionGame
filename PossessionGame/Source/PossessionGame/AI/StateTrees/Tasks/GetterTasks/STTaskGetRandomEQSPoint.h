#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskGetRandomEQSPoint.generated.h"

UCLASS(DisplayName = "Get Random EQS Point", Category = "Getter Tasks")
class POSSESSIONGAME_API USTTaskGetRandomEQSPoint : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
