#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUI.generated.h"

class UInvalidationBox;
class UMainMenuButtonUI;
class UConfirmationPopupUI;

UCLASS()
class POSSESSIONGAME_API UMainMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	UFUNCTION()
	void CloseUI();
private:
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
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> MainMenuFadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FirstLaunchAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FirstLaunchAnimDelegate;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void SetupButtons();
	void TriggerMainMenuFadeInAnim();
	void TriggerMainMenuFadeOutAnim();
	void ToggleMenuButtons(const bool bState) const;
	UFUNCTION()
	void Play();
	UFUNCTION()
	void OpenSettings();
	UFUNCTION()
	void QuitGame();
	UFUNCTION()
	void TriggerQuitPopupFadeIn();
	UFUNCTION()
	void TriggerQuitPopupFadeOut();
};
