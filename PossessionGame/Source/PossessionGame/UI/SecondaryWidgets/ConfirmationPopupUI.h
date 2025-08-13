#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmationPopupUI.generated.h"

class UMainMenuButtonUI;
class UButton;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UConfirmationPopupUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UMainMenuButtonUI> GetAcceptButton() { return AcceptButton; }
	TObjectPtr<UMainMenuButtonUI> GetDeclineButton() { return DeclineButton; }

	void PlayPopupFadeInAnim(const bool bState);
	void SetTitleText(const FText& InText) const;
private:
	UPROPERTY(EditAnywhere, Category = "Confirmation Settings", meta = (MultiLine, AllowPrivateAccess = true))
	FText ConfirmationTitleText;
	UPROPERTY(EditAnywhere, Category = "Confirmation Settings", meta = (AllowPrivateAccess = true))
	FText AcceptText;
	UPROPERTY(EditAnywhere, Category = "Confirmation Settings", meta = (AllowPrivateAccess = true))
	FText DeclineText;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ConfirmationTitle;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> AcceptButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> DeclineButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopupFadeIn;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	UFUNCTION()
	void FadeOutAnimFinished();
};
