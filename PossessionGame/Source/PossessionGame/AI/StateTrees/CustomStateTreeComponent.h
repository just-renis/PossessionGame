#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeComponent.h"
#include "CustomStateTreeComponent.generated.h"

enum class EStateTreeType : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class POSSESSIONGAME_API UCustomStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()
public:
	TObjectPtr<UCustomStateTreeComponent> GetTaskStateTree(EStateTreeType Id);
	
	virtual void Cleanup() override;
	void SetStateTree(TObjectPtr<UStateTree>& InStateTree);
	void RegisterTaskStateTree(EStateTreeType Id, TObjectPtr<UCustomStateTreeComponent>& TaskStateTree);
private:
	UPROPERTY()
	TMap<EStateTreeType, TObjectPtr<UCustomStateTreeComponent>> TaskStateTrees;

	void Destroy();
};
