#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AlertPopupUI.generated.h"

class UMainMenuButtonUI;
class UTextBlock;
class UButton;

UCLASS()
class POSSESSIONGAME_API UAlertPopupUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UMainMenuButtonUI> GetAlertButton() { return AlertButton; }

	void PlayPopupFadeInAnim(const bool bState);
	void SetText(const FText& InText) const;
private:
	UPROPERTY(EditAnywhere, Category = "Alert Settings", meta = (AllowPrivateAccess = true))
	FText AlertTitleText;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> AlertTitle;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> AlertButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopupFadeInAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent AlertPopupFadeOutDelegate;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	UFUNCTION()
	void AlertPopupFadeOutFinished();
};
