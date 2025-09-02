#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskMinionBossStomp.generated.h"

class AMinionBossAIController;

UCLASS(DisplayName = "Stomp", Category = "Character Tasks | Minion Boss Tasks")
class POSSESSIONGAME_API USTTaskMinionBossStomp : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AMinionBossAIController> CachedController;
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void StompFinished();
};
