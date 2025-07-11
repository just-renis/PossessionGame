#include "AudioSettingsUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/ELanguageCode.h"
#include "PossessionGame/UI/SecondaryWidgets/SettingsOptionUI.h"
#include "PossessionGame/UI/SecondaryWidgets/SliderUI.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameUserSettings.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/SettingsSave.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"

void UAudioSettingsUI::UpdateDefaultMainSettings()
{
	const TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0));
	ApplyMasterVolumeValue(SavedSettings->GetMasterVolumeValue());
	ApplySfxVolumeValue(SavedSettings->GetSfxVolumeValue());
	ApplyMusicVolumeValue(SavedSettings->GetMusicVolumeValue());
	ApplyHorizontalSensitivityValue(SavedSettings->GetMouseHorizontalSensitivity());
	ApplyVerticalSensitivityValue(SavedSettings->GetMouseVerticalSensitivity());
	ApplyFpsCapValue(Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings())->GetFrameRateLimit());
}

void UAudioSettingsUI::ApplySettings()
{
	const TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0));
	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->SaveValue();
		SavedSettings->SetMasterVolume(MasterVolumeSlider->GetSavedValue());
	}
	if (SfxVolumeSlider)
	{
		SfxVolumeSlider->SaveValue();
		SavedSettings->SetSfxVolume(GetWorld(), SfxVolumeSlider->GetSavedValue());
	}
	if (MusicVolumeSlider)
	{
		MusicVolumeSlider->SaveValue();
		SavedSettings->SetMusicVolume(GetWorld(), MusicVolumeSlider->GetSavedValue());
	}
	if (HorizontalSensitivitySlider)
	{
		HorizontalSensitivitySlider->SaveValue();
		SavedSettings->SetMouseHorizontalSensitivity(HorizontalSensitivitySlider->GetSavedValue());
	}
	if (VerticalSensitivitySlider)
	{
		VerticalSensitivitySlider->SaveValue();
		SavedSettings->SetMouseVerticalSensitivity(VerticalSensitivitySlider->GetSavedValue());
	}
	if (FpsCapSlider) FpsCapSlider->SaveValue();
	const FString& LanguageEncode = GetLanguageOption(LanguageCode).LanguageCode;
	SavedSettings->SetLanguage(LanguageEncode);
	FInternationalization::Get().SetCurrentCulture(LanguageEncode);
	UGameplayStatics::SaveGameToSlot(SavedSettings, "SettingsSlot", 0);
}

void UAudioSettingsUI::SetSavedSettings()
{
	if (MasterVolumeSlider) ApplyMasterVolumeValue(MasterVolumeSlider->GetSavedValue());
	if (SfxVolumeSlider) ApplySfxVolumeValue(SfxVolumeSlider->GetSavedValue());
	if (MusicVolumeSlider) ApplyMusicVolumeValue(MusicVolumeSlider->GetSavedValue());
	if (HorizontalSensitivitySlider) ApplyHorizontalSensitivityValue(HorizontalSensitivitySlider->GetSavedValue());
	if (VerticalSensitivitySlider) ApplyVerticalSensitivityValue(VerticalSensitivitySlider->GetSavedValue());
	if (FpsCapSlider) ApplyFpsCapValue(FpsCapSlider->GetSavedValue());
}

ELanguageCode UAudioSettingsUI::GetLanguageCode(const FString& InCode)
{
	if (InCode.Equals("lt")) return ELanguageCode::Lt;
	return ELanguageCode::En;
}

FText UAudioSettingsUI::GetLanguageNameByCode(const FString& InCode)
{
	if (InCode.Equals("lt")) return FText::FromString("Lithuanian");
	return FText::FromString("English");
}

FLanguageOption UAudioSettingsUI::GetLanguageOption(const ELanguageCode InLanguageCode)
{
	if (const FLanguageOption* LanguageOptionData = Languages.Find(InLanguageCode)) return *LanguageOptionData;
	return FLanguageOption();
}

void UAudioSettingsUI::NativeConstruct()
{
	Super::NativeConstruct();
	#if WITH_EDITOR
		if (ResetSaveSlotButton)
		{
			ResetSaveSlotButton->OnClicked.AddDynamic(this, &UAudioSettingsUI::ResetSaveSlot);
			ResetSaveSlotButton->SetVisibility(ESlateVisibility::Visible);
		}
	#endif
	if (!LanguageOption) return;
	const TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0));
	LanguageCode = GetLanguageCode(SavedSettings->GetCurrentLanguage());
	SetupLanguagesData();
	LanguageOption->SetText(GetLanguageOption(LanguageCode).LanguageName);
	LanguageOption->GetDownscaleButton()->OnClicked.AddDynamic(this, &UAudioSettingsUI::LanguageSwitchLeft);
	LanguageOption->GetUpscaleButton()->OnClicked.AddDynamic(this, &UAudioSettingsUI::LanguageSwitchRight);
	if (FpsCapSlider) FpsCapSlider->GetSlider()->OnValueChanged.AddDynamic(this, &UAudioSettingsUI::SetFpsValue);
	if (HorizontalSensitivitySlider) HorizontalSensitivitySlider->GetSlider()->OnValueChanged.AddDynamic(this, &UAudioSettingsUI::SetHorizontalSensitivityNumberValue);
	if (VerticalSensitivitySlider) VerticalSensitivitySlider->GetSlider()->OnValueChanged.AddDynamic(this, &UAudioSettingsUI::UAudioSettingsUI::SetVerticalSensitivityNumberValue);
}

void UAudioSettingsUI::ApplyMasterVolumeValue(const float InValue) const
{
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetMasterVolume(InValue);
	if (MasterVolumeSlider) MasterVolumeSlider->SetValue(InValue);
}

void UAudioSettingsUI::ApplySfxVolumeValue(const float InValue) const
{
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetSfxVolume(GetWorld(), InValue);
	if (SfxVolumeSlider) SfxVolumeSlider->SetValue(InValue);
}

void UAudioSettingsUI::ApplyMusicVolumeValue(const float InValue) const
{
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetMusicVolume(GetWorld(), InValue);
	if (MusicVolumeSlider) MusicVolumeSlider->SetValue(InValue);
}

void UAudioSettingsUI::SetupLanguagesData()
{
	TArray<FString> AvailableCultures = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Game);
	for (const FString& CultureName : AvailableCultures) Languages.Add(GetLanguageCode(CultureName), FLanguageOption(GetLanguageNameByCode(CultureName), CultureName));
}

void UAudioSettingsUI::ApplyHorizontalSensitivityValue(const float InValue)
{
	GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->SetMouseHorizontalSensitivity(InValue);
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetMouseHorizontalSensitivity(InValue);
	if (!HorizontalSensitivitySlider) return;
	HorizontalSensitivitySlider->SetValue(InValue);
	SetHorizontalSensitivityNumberValue(InValue);
}

void UAudioSettingsUI::ApplyVerticalSensitivityValue(const float InValue)
{
	GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->SetMouseVerticalSensitivity(InValue);
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetMouseVerticalSensitivity(InValue);
	if (!VerticalSensitivitySlider) return;
	VerticalSensitivitySlider->SetValue(InValue);
	SetVerticalSensitivityNumberValue(InValue);
}

void UAudioSettingsUI::ApplyFpsCapValue(const float InValue) const
{
	const TObjectPtr<UPossessionGameUserSettings> GameUserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings());
	GameUserSettings->SetFrameRateLimit(InValue);
	if (FpsCapNumberText) FpsCapNumberText->SetText(FText::AsNumber(static_cast<int32>(GameUserSettings->GetFrameRateLimit())));
	if (FpsCapSlider) FpsCapSlider->SetValue(InValue);
}

void UAudioSettingsUI::ResetSaveSlot() { if (GetGameInstance()) Cast<UPossessionGameInstance>(GetGameInstance())->ResetSaveSlot(); }

void UAudioSettingsUI::LanguageSwitchLeft()
{
	uint8 NextValue = static_cast<uint8>(LanguageCode) + 1;
	if (!Languages.Contains(static_cast<ELanguageCode>(NextValue))) NextValue = 0;
	LanguageCode = static_cast<ELanguageCode>(NextValue);
	if (LanguageOption) LanguageOption->SetText(Languages[LanguageCode].LanguageName);
}

void UAudioSettingsUI::LanguageSwitchRight()
{
	uint8 NextValue = static_cast<uint8>(LanguageCode) - 1;
	if (!Languages.Contains(static_cast<ELanguageCode>(NextValue))) NextValue = Languages.Num() - 1;
	LanguageCode = static_cast<ELanguageCode>(NextValue);
	if (LanguageOption) LanguageOption->SetText(Languages[LanguageCode].LanguageName);
}

void UAudioSettingsUI::SetFpsValue(const float InValue)
{
	const TObjectPtr<UPossessionGameUserSettings> GameUserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings());
	GameUserSettings->SetFrameRateLimit(InValue);
	if (FpsCapNumberText) FpsCapNumberText->SetText(FText::AsNumber(static_cast<int32>(GameUserSettings->GetFrameRateLimit())));
}

void UAudioSettingsUI::SetHorizontalSensitivityNumberValue(const float InValue)
{
	if (!HorizontalSensitivityNumber) return;
	FNumberFormattingOptions Options;
	Options.SetMinimumFractionalDigits(2).SetMaximumFractionalDigits(2);
	HorizontalSensitivityNumber->SetText(FText::AsNumber(InValue, &Options));
}

void UAudioSettingsUI::SetVerticalSensitivityNumberValue(const float InValue)
{
	if (!VerticalSensitivityNumber) return;
	FNumberFormattingOptions Options;
	Options.SetMinimumFractionalDigits(2).SetMaximumFractionalDigits(2);
	VerticalSensitivityNumber->SetText(FText::AsNumber(InValue, &Options));
}
