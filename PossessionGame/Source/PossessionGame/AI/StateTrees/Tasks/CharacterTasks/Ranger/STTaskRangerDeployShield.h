#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskRangerDeployShield.generated.h"

UCLASS(DisplayName = "Deploy Shield", Category = "Character Tasks | Ranger Tasks")
class POSSESSIONGAME_API USTTaskRangerDeployShield : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	UFUNCTION()
	void DeployShieldFinished();
};
