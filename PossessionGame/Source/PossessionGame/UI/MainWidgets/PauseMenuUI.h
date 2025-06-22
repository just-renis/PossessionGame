#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuUI.generated.h"

class UConfirmationPopupUI;
class UMainMenuButtonUI;
class UTextBlock;
class UButton;

UCLASS()
class POSSESSIONGAME_API UPauseMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	UFUNCTION()
	void CloseUI();
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ContinueButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> SettingsButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ReturnToMenuButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UConfirmationPopupUI> ReturnToMainMenuPopup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutAnimDelegate;

	virtual void NativeConstruct() override;
	void Toggle(bool bState);
	void SetupButtons();
	void SetButtonsState(const bool bState) const;
	UFUNCTION()
	void OpenSettings();
	UFUNCTION()
	void ReturnToMainMenu();
	UFUNCTION()
	void TriggerReturnToMainMenuPopupFadeIn();
	UFUNCTION()
	void TriggerReturnToMainMenuPopupFadeOut();
	UFUNCTION()
	void FadeOutFinished();
};
