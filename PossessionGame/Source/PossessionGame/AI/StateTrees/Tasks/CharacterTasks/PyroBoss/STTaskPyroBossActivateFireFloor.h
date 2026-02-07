#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskPyroBossActivateFireFloor.generated.h"

class APyroBossAIController;

UCLASS(DisplayName = "Activate Fire Floor", Category = "Character Tasks | Pyro Boss Tasks")
class POSSESSIONGAME_API USTTaskPyroBossActivateFireFloor : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<APyroBossAIController> CachedController;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void ActivateFireFloorFinished();
};
