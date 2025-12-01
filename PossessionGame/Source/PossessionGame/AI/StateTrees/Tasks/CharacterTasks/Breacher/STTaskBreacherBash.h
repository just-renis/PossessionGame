#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskBreacherBash.generated.h"

class ABreacherAIController;

UCLASS(DisplayName = "Bash", Category = "Character Tasks | Breacher Tasks")
class POSSESSIONGAME_API USTTaskBreacherBash : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ABreacherAIController> CachedController;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void BashFinished();
};
