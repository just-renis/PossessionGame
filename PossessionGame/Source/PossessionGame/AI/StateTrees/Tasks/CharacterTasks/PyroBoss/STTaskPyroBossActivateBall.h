#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskPyroBossActivateBall.generated.h"

class APyroBossAIController;

UCLASS(DisplayName = "Ball", Category = "Character Tasks | Pyro Boss Tasks")
class POSSESSIONGAME_API USTTaskPyroBossActivateBall : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<APyroBossAIController> CachedController;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void BallPhaseFinished();
};
