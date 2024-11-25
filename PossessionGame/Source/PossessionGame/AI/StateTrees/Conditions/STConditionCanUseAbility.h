#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"
#include "STConditionCanUseAbility.generated.h"

UCLASS(DisplayName = "Can Use Ability", Category = "Custom Conditions")
class POSSESSIONGAME_API USTConditionCanUseAbility : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 AbilityIndex;

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
