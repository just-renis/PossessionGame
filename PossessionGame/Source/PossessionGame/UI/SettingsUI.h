#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsUI.generated.h"

class UInvalidationBox;
class UAlertPopupUI;
class UControlsSettingsUI;
class UAudioSettingsUI;
class UGraphicsSettingsUI;
class UWidgetSwitcher;
class UButton;

UCLASS()
class POSSESSIONGAME_API USettingsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	UFUNCTION()
	void CloseUI();
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ApplySettingsButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> CloseButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> GraphicsTabButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> AudioTabButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ControlsTabButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> LanguageTabButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UWidgetSwitcher> TabsSwitcher;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UGraphicsSettingsUI> GraphicsSettings;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UAudioSettingsUI> AudioSettings;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UControlsSettingsUI> ControlsSettings;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ResetKeyBindsButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UAlertPopupUI> DuplicateKeyBindsAlert;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInvalidationBox> TabsInv;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInvalidationBox> SettingsInv;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInvalidationBox> CloseButtonInv;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInvalidationBox> ApplySettingsButtonInv;
	UPROPERTY(Transient, meta = (BindWidgetAnim, AllowPrivateAccess = true))
	TObjectPtr<UWidgetAnimation> SettingsFadeIn;
	
	virtual void NativeConstruct() override;
	void SetupUIEvents();
	void UpdateDefaultSettings();
	void SetSavedSettings();
	UFUNCTION()
	void ApplySettings();
	UFUNCTION()
	void OpenGraphicsTab();
	UFUNCTION()
	void OpenAudioTab();
	UFUNCTION()
	void OpenControlsTab();
	UFUNCTION()
	void OpenLanguageTab();
	UFUNCTION()
	void ResetKeyBindsToDefault();
	UFUNCTION()
	void CloseDuplicateKeyBindsPopup();
};
