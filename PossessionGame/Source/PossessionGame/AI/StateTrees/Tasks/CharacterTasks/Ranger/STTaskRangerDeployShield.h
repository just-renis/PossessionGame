#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskRangerDeployShield.generated.h"

class ARangerAIController;

UCLASS(DisplayName = "Deploy Shield", Category = "Character Tasks | Ranger Tasks")
class POSSESSIONGAME_API USTTaskRangerDeployShield : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ARangerAIController> CachedController;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void DeployShieldFinished();
};
