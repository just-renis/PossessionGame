#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskRangerGrenadeLaunch.generated.h"

UCLASS(DisplayName = "Grenade Launch", Category = "Character Tasks | Ranger Tasks")
class POSSESSIONGAME_API USTTaskRangerGrenadeLaunch : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	UFUNCTION()
	void GrenadeLaunchFinished();
};
