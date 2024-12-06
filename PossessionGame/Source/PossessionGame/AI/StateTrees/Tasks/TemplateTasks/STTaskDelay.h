#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskDelay.generated.h"

UCLASS(DisplayName = "Delay", Category = "Template Tasks")
class POSSESSIONGAME_API USTTaskDelay : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	bool bUseCustomDelay = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bUseUnlimitedDelay = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float Delay = 1.f;
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	UFUNCTION()
	virtual void OnDelayCompleted();
};
