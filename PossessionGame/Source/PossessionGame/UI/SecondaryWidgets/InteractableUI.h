#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractableUI.generated.h"

class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UInteractableUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractableText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;

	virtual void NativeConstruct() override;
	void UpdateInteractableText(const FName& InKeyBindName, const FText& InKeyBindText);
};
