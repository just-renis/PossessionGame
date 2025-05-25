#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsUI.generated.h"

class UKeyBindingUI;
class UAlertPopupUI;
class UInvalidationBox;
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
	void BeginRebind(const TObjectPtr<UKeyBindingUI>& InTargetKeyWidget);
	void OpenUI();
	UFUNCTION()
	void CloseUI();
private:
	USettingsUI(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ApplySettingsButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> CloseButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> GraphicsTabButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> MainTabButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ControlsTabButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UWidgetSwitcher> TabsSwitcher;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UGraphicsSettingsUI> GraphicsSettings;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UAudioSettingsUI> MainSettings;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UControlsSettingsUI> ControlsSettings;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ResetKeyBindsButton;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInvalidationBox> TabsInv;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInvalidationBox> SettingsInv;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInvalidationBox> CloseButtonInv;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInvalidationBox> ApplySettingsButtonInv;
//	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	//TObjectPtr<UAlertPopupUI> DuplicateKeyBindsAlert;
	UPROPERTY(Transient, meta = (BindWidgetAnim, AllowPrivateAccess = true))
	TObjectPtr<UWidgetAnimation> SettingsFadeIn;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> RebindPanelInv;
	UPROPERTY()
	TObjectPtr<UKeyBindingUI> CurrentKeyWidget;
	
	virtual void NativeConstruct() override;
	void SetupUIEvents();
	void UpdateDefaultSettings();
	void SetSavedSettings();
	void OpenDuplicateKeyBindsPopup() const;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	UFUNCTION()
	void ApplySettings();
	UFUNCTION()
	void OpenMainTab();
	UFUNCTION()
	void OpenGraphicsTab();
	UFUNCTION()
	void OpenControlsTab();
	UFUNCTION()
	void ResetKeyBindsToDefault();
	UFUNCTION()
	void CloseDuplicateKeyBindsPopup();
};
