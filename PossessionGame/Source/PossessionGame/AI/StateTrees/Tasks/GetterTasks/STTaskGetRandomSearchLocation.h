#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskGetRandomSearchLocation.generated.h"

UCLASS(DisplayName = "Get Random Search Location", Category = "Getter Tasks")
class POSSESSIONGAME_API USTTaskGetRandomSearchLocation : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowedPrivateAccess = true))
	float SearchRadius = 500.f;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
