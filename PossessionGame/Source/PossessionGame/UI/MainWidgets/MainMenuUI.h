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
	bool MainMenuFadeOutState = false;
	bool QuitFadeInState = false;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UMainMenuButtonUI> PlayButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UMainMenuButtonUI> SettingsButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UMainMenuButtonUI> QuitButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UConfirmationPopupUI> QuitConfirmationPopup;
	UPROPERTY(Transient, meta = (BindWidgetAnim, AllowPrivateAccess = true))
	TObjectPtr<UWidgetAnimation> MainMenuFadeOut;
	UPROPERTY(Transient, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInvalidationBox> MainMenuInv;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void SetupButtons();
	void TriggerMainMenuFadeOutAnim();
	void TriggerQuitFadeInAnim();
	UFUNCTION()
	void Play();
	UFUNCTION()
	void OpenSettings();
	UFUNCTION()
	void QuitGame();
	UFUNCTION()
	void TriggerQuitPopup();
};
