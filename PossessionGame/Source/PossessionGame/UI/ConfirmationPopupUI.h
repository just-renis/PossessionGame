#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmationPopupUI.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UConfirmationPopupUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UButton> GetAcceptButton() { return AcceptButton; }
	TObjectPtr<UButton> GetDeclineButton() { return DeclineButton; }

	void PlayPopupFadeInAnim(bool bState);
private:
	UPROPERTY(EditAnywhere, Category = "Confirmation Settings", meta = (AllowPrivateAccess = true))
	FText ConfirmationTitleText;
	UPROPERTY(EditAnywhere, Category = "Confirmation Settings", meta = (AllowPrivateAccess = true))
	FText AcceptText;
	UPROPERTY(EditAnywhere, Category = "Confirmation Settings", meta = (AllowPrivateAccess = true))
	FText DeclineText;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ConfirmationTitle;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> AcceptButtonText;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> DeclineButtonText;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> AcceptButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UButton> DeclineButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopupFadeIn;

	virtual void NativePreConstruct() override;
};
