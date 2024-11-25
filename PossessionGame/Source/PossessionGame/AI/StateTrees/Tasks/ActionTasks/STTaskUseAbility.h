#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTaskUseAbility.generated.h"

UCLASS(DisplayName = "Use Ability", Category = "Action Tasks")
class POSSESSIONGAME_API USTTaskUseAbility : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowedPrivateAccess = true))
	int32 AbilitySlot = 0;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
};
