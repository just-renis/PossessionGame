#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskRangerGrenadeLaunch.generated.h"

class ARangerAIController;

UCLASS(DisplayName = "Grenade Launch", Category = "Character Tasks | Ranger Tasks")
class POSSESSIONGAME_API USTTaskRangerGrenadeLaunch : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ARangerAIController> CachedController;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void GrenadeLaunchFinished();
};
