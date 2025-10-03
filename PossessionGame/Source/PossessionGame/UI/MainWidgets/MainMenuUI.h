#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUI.generated.h"

class UModeSelectUI;
class UInvalidationBox;
class UMainMenuButtonUI;
class UConfirmationPopupUI;

UCLASS()
class POSSESSIONGAME_API UMainMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void CloseUI();
	UFUNCTION()
	void TriggerMainMenuFadeInAnim();
private:
	bool bIsQuitPopupOpen = false;
	bool bIsModeSelectionOpen = false;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> PlayButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> SettingsButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> QuitButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UConfirmationPopupUI> QuitConfirmationPopup;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> MainMenuInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> BlackScreenInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UModeSelectUI> ModeSelection;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> MainMenuFadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FirstLaunchAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PulsatingAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FirstLaunchAnimDelegate;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void SetupButtons();
	void TriggerMainMenuFadeOutAnim();
	void ToggleMenuButtons(const bool bState) const;
	UFUNCTION()
	void TriggerSelectionMenuFadeOut();
	UFUNCTION()
	void TriggerSelectionMenuFadeIn();
	UFUNCTION()
	void OpenSettings();
	UFUNCTION()
	void QuitGame();
	UFUNCTION()
	void TriggerQuitPopupFadeIn();
	UFUNCTION()
	void TriggerQuitPopupFadeOut();
};
