#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "KeyBindingUI.generated.h"

class USettingsUI;
class UTextBlock;
class USettingsTitleUI;
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
	void ResetKeyBind();
	UFUNCTION()
	void OnKeySelected(FInputChord InMapping);
private:
	UPROPERTY(EditAnywhere, Category = "Key Binding", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsTitleUI> Title;
	UPROPERTY(EditAnywhere, Category = "Key Binding", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> InputKeySelectorButton;
	UPROPERTY(EditAnywhere, Category = "Key Binding", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ResetKeyBindingButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KeyBindText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ResetHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ResetSelectAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> KeyBindHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> KeyBindSelectAnim;
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
	UFUNCTION()
	void ResetSingularKeyBind();
	UFUNCTION()
	void PlayResetButtonSelectAnim();
	UFUNCTION()
	void PlayResetHoverAnim();
	UFUNCTION()
	void PlayResetUnHoverAnim();
	UFUNCTION()
	void PlayKeyBindButtonSelectAnim();
	UFUNCTION()
	void PlayKeyBindHoverAnim();
	UFUNCTION()
	void PlayKeyBindUnHoverAnim();
};
