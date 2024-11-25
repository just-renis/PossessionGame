#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskRotateToLocation.generated.h"

UCLASS(DisplayName = "Rotate To Location", Category = "Movement Tasks")
class POSSESSIONGAME_API USTTaskRotateToLocation : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> Target;
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
