#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CloseButtonUI.generated.h"

class UButton;

UCLASS()
class POSSESSIONGAME_API UCloseButtonUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UButton>& Get() { return Button; }
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button;
};
