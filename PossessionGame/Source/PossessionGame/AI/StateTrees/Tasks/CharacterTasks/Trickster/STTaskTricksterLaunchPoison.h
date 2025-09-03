#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskTricksterLaunchPoison.generated.h"

class ATricksterAIController;

UCLASS(DisplayName = "Launch Poison", Category = "Character Tasks | Trickster Tasks")
class POSSESSIONGAME_API USTTaskTricksterLaunchPoison : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ATricksterAIController> CachedController;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void PoisonGrenadeLaunchFinished();
};
