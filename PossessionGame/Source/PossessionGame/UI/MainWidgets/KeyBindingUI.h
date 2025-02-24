#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "KeyBindingUI.generated.h"

class USettingsTitleUI;
class UInputKeySelector;
class UButton;

UCLASS()
class POSSESSIONGAME_API UKeyBindingUI : public UUserWidget
{
	GENERATED_BODY()
public:
	FKey& GetKeyBindKey() { return TempSelectedKey; }
	
	void Setup(TObjectPtr<UEnhancedInputUserSettings> InUserSettings, FPlayerKeyMapping InMapping);
	void RemapKeyBind();
	void SetSavedKeyBind();
	UFUNCTION()
	void ResetKeyBind();
private:
	UPROPERTY(EditAnywhere, Category = "Key Binding", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsTitleUI> Title;
	UPROPERTY(EditAnywhere, Category = "Key Binding", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UInputKeySelector> InputKeySelector;
	UPROPERTY(EditAnywhere, Category = "Key Binding", meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UButton> ResetKeyBindingButton;
	UPROPERTY()
	TObjectPtr<UEnhancedInputUserSettings> UserSettings;
	UPROPERTY()
	FPlayerKeyMapping Mapping;
	UPROPERTY()
	FKey TempSelectedKey;

	virtual void NativeConstruct() override;
	void SetupEvents();
	UFUNCTION()
	void OnKeySelected(FInputChord InMapping);
};
