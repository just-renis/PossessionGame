#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskWraithMeteorShower.generated.h"

class AWraithAIController;

UCLASS(DisplayName = "Meteor Shower", Category = "Character Tasks | Wraith Tasks")
class POSSESSIONGAME_API USTTaskWraithMeteorShower : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AWraithAIController> CachedController;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void MeteorShowerFinished();
};
