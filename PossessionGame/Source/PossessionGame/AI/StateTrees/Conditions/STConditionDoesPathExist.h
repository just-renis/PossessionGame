#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"
#include "STConditionDoesPathExist.generated.h"

UENUM()
namespace EPathFindingType
{
	enum Type : int
	{
		NavmeshRaycast2D UMETA(ToolTip = "Really Fast"),
		HierarchicalQuery UMETA(ToolTip = "Fast"),
		RegularPathFinding UMETA(ToolTip = "Slow"),
	};
}

UCLASS(DisplayName = "Does Path Exist", Category = "Custom Conditions")
class POSSESSIONGAME_API USTConditionDoesPathExist : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()
	
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TEnumAsByte<EPathFindingType::Type> PathQueryType;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bInvert = false;
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (AllowPrivateAccess = true))
	FVector TargetLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> Target = nullptr;
};
