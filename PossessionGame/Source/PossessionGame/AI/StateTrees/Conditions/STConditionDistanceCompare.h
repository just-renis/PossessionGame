#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"
#include "STConditionDistanceCompare.generated.h"

UCLASS(DisplayName = "Distance Compare", Category = "Custom Conditions")
class POSSESSIONGAME_API USTConditionDistanceCompare : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bCheckWithTargetObject = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bInvert = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float Distance = 0.f;
	UPROPERTY(EditAnywhere, Category = "Condition")
	EGenericAICheck Operator = EGenericAICheck::Equal;
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (AllowPrivateAccess = true))
	FVector TargetLocation;
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> Target;

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	bool ConditionResult(float DistanceBetweenLocations) const;
};
