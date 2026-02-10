#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskPyroBossStomp.generated.h"

class APyroBossAIController;

UCLASS(DisplayName = "Stomp", Category = "Character Tasks | Pyro Boss Tasks")
class POSSESSIONGAME_API USTTaskPyroBossStomp : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<APyroBossAIController> CachedController;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void ActivateStompFinished();
};
