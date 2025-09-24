#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskWardenBeamShot.generated.h"

class AWardenAIController;

UCLASS(DisplayName = "Beam Shot", Category = "Character Tasks | Warden Tasks")
class POSSESSIONGAME_API USTTaskWardenBeamShot : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<AWardenAIController> CachedController;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void BeamShot();
};
