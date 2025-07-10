#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/ELanguageCode.h"
#include "AudioSettingsUI.generated.h"

enum class ELanguageCode : uint8;
class USettingsOptionUI;
class UButton;
class USliderUI;

USTRUCT()
struct POSSESSIONGAME_API FLanguageOption
{
	GENERATED_BODY()

	FLanguageOption() : LanguageName(), LanguageCode() {}
	FLanguageOption(const FText& InLanguageName, const FString& InLanguageCode) : LanguageName(InLanguageName), LanguageCode(InLanguageCode) {}

	UPROPERTY()
	FText LanguageName = FText::FromString("English");
	UPROPERTY()
	FString LanguageCode = "en";
};

UCLASS()
class POSSESSIONGAME_API UAudioSettingsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateDefaultMainSettings() const;
	void ApplySettings();
	void SetSavedSettings() const;
private:
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
	TObjectPtr<UButton> ResetSaveSlotButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USettingsOptionUI> LanguageOption;

	ELanguageCode GetLanguageCode(const FString& InCode);
	FText GetLanguageNameByCode(const FString& InCode);
	FLanguageOption GetLanguageOption(const ELanguageCode InLanguageCode);

	virtual void NativeConstruct() override;
	void ApplyMasterVolumeValue(const float InValue) const;
	void ApplySfxVolumeValue(const float InValue) const;
	void ApplyMusicVolumeValue(const float InValue) const;
	void SetupLanguagesData();
	void ApplyHorizontalSensitivityValue(const float InValue) const;
	void ApplyVerticalSensitivityValue(const float InValue) const;
	UFUNCTION()
	void ResetSaveSlot();
	UFUNCTION()
	void LanguageSwitchLeft();
	UFUNCTION()
	void LanguageSwitchRight();
};
