#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "PossessionGame/Utilities/Data Utilities/FStateTreeData.h"
#include "STTaskRunStateTree.generated.h"

enum class EStateTreeType : uint8;
class UCustomStateTreeComponent;

UCLASS(DisplayName = "Run State Tree", Category = "Template Tasks")
class POSSESSIONGAME_API USTTaskRunStateTree : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EStateTreeType Id;
	UPROPERTY(EditAnywhere, Category = "Context", meta = (AllowPrivateAccess = true))
	FStateTreeData StateTreeData;
	UPROPERTY()
	TObjectPtr<UCustomStateTreeComponent> StateTree;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStateTree> StateTreeAsset;
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	void InitializeStateTree(FStateTreeExecutionContext& Context);
};
