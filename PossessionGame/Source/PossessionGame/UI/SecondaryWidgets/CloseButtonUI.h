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
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ClickAnim;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnHovered();
	UFUNCTION()
	void OnUnHovered();
	UFUNCTION()
	void OnClicked();
};
