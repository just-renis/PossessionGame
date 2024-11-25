#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskDelay.generated.h"

UCLASS(DisplayName = "Delay", Category = "Template Tasks")
class POSSESSIONGAME_API USTTaskDelay : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
protected:
	bool bUseCustomDelay = false;
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	UFUNCTION()
	virtual void OnDelayCompleted();
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float Delay = 1.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bUseUnlimitedDelay = false;
};
