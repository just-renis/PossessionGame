#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskMinionBossStomp.generated.h"

UCLASS(DisplayName = "Stomp", Category = "Character Tasks | Minion Boss Tasks")
class POSSESSIONGAME_API USTTaskMinionBossStomp : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	UFUNCTION()
	void StompFinished();
};
