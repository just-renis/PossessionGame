#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/ELanguageCode.h"
#include "AudioSettingsUI.generated.h"

class UTextBlock;
enum class ELanguageCode : uint8;
class USettingsOptionUI;
class USliderUI;

USTRUCT()
struct POSSESSIONGAME_API FLanguageOption
{
	GENERATED_BODY()

	FLanguageOption() : LanguageName(), LanguageCode() {}
	FLanguageOption(const FText& InLanguageName, const FString& InLanguageCode) : LanguageName(InLanguageName), LanguageCode(InLanguageCode) {}

	UPROPERTY()
	FText LanguageName = NSLOCTEXT("ST_Localization", "English", "English");
	UPROPERTY()
	FString LanguageCode = "en";
};

UCLASS()
class POSSESSIONGAME_API UAudioSettingsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateDefaultMainSettings();
	void ApplySettings();
	void SetSavedSettings();
private:
	bool bIsRunToggleMode;
	ELanguageCode LanguageCode;
	UPROPERTY()
	TMap<ELanguageCode, FLanguageOption> Languages;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderUI> MasterVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderUI> MusicVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderUI> SfxVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderUI> HorizontalSensitivitySlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderUI> VerticalSensitivitySlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderUI> FpsCapSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USettingsOptionUI> LanguageOption;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USettingsOptionUI> RunModeOption;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FpsCapNumberText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HorizontalSensitivityNumber;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> VerticalSensitivityNumber;

	ELanguageCode GetLanguageCode(const FString& InCode);
	FText GetLanguageNameByCode(const FString& InCode);
	FLanguageOption GetLanguageOption(const ELanguageCode InLanguageCode);

	virtual void NativeConstruct() override;
	void ApplyMasterVolumeValue(const float InValue) const;
	void ApplySfxVolumeValue(const float InValue) const;
	void ApplyMusicVolumeValue(const float InValue) const;
	void SetupLanguagesData();
	void ApplyHorizontalSensitivityValue(const float InValue);
	void ApplyVerticalSensitivityValue(const float InValue);
	void ApplyFpsCapValue(const float InValue) const;
	void ApplyRunToggleValue(const bool bState);
	UFUNCTION()
	void ToggleRunMode();
	UFUNCTION()
	void LanguageSwitchLeft();
	UFUNCTION()
	void LanguageSwitchRight();
	UFUNCTION()
	void SetFpsValue(const float InValue);
	UFUNCTION()
	void SetHorizontalSensitivityNumberValue(const float InValue);
	UFUNCTION()
	void SetVerticalSensitivityNumberValue(const float InValue);
};
