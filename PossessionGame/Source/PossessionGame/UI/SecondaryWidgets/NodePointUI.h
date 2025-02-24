#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NodePointUI.generated.h"

enum class ENodeType : uint8;
class UButton;

UCLASS()
class POSSESSIONGAME_API UNodePointUI : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> NodeButton;
	UPROPERTY(EditAnywhere, Category = "Node Type", meta = (AllowPrivateAccess = true))
	ENodeType NodeType;
};
