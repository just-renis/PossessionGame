#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "KeyBindingUI.generated.h"

class USettingsUI;
class UControlsSettingsUI;
class UTextBlock;
class USettingsTitleUI;
class UInputKeySelector;
class UButton;

UCLASS()
class POSSESSIONGAME_API UKeyBindingUI : public UUserWidget
{
	GENERATED_BODY()
public:
	FKey& GetKeyBindKey() { return TempSelectedKey; }
	
	void Setup(const TObjectPtr<UEnhancedInputUserSettings>& InUserSettings, const FPlayerKeyMapping& InMapping, const TObjectPtr<USettingsUI>& InParentWidget);
	void RemapKeyBind();
	void SetSavedKeyBind() const;
	void CancelRebind() const;
	UFUNCTION()
	void OnKeySelected(FInputChord InMapping);
	UFUNCTION()
	void ResetKeyBind();
private:
	UPROPERTY(EditAnywhere, Category = "Key Binding", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsTitleUI> Title;
	//UPROPERTY(EditAnywhere, Category = "Key Binding", meta = (BindWidget, AllowPrivateAccess = true))
	//TObjectPtr<UInputKeySelector> InputKeySelector;
	UPROPERTY(EditAnywhere, Category = "Key Binding", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> InputKeySelector;
	UPROPERTY(EditAnywhere, Category = "Key Binding", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ResetKeyBindingButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KeyBindText;
	UPROPERTY()
	TObjectPtr<UEnhancedInputUserSettings> UserSettings;
	UPROPERTY()
	FPlayerKeyMapping Mapping;
	UPROPERTY()
	FKey TempSelectedKey;
	UPROPERTY()
	TObjectPtr<USettingsUI> ParentWidget;

	FText GetDisplayName(const FKey& InKeyName) const;

	virtual void NativeConstruct() override;
	void SetupEvents();
	UFUNCTION()
	void OnSelectingKeyChanged();
};
