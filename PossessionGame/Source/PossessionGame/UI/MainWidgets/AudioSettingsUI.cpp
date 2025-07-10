#include "AudioSettingsUI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/ELanguageCode.h"
#include "PossessionGame/UI/SecondaryWidgets/SettingsOptionUI.h"
#include "PossessionGame/UI/SecondaryWidgets/SliderUI.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/SettingsSave.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"

void UAudioSettingsUI::UpdateDefaultMainSettings() const
{
	const TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0));
	ApplyMasterVolumeValue(SavedSettings->GetMasterVolumeValue());
	ApplySfxVolumeValue(SavedSettings->GetSfxVolumeValue());
	ApplyMusicVolumeValue(SavedSettings->GetMusicVolumeValue());
	ApplyHorizontalSensitivityValue(SavedSettings->GetMouseHorizontalSensitivity());
	ApplyVerticalSensitivityValue(SavedSettings->GetMouseVerticalSensitivity());
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
	const FString& LanguageEncode = GetLanguageOption(LanguageCode).LanguageCode;
	SavedSettings->SetLanguage(LanguageEncode);
	FInternationalization::Get().SetCurrentCulture(LanguageEncode);
	UGameplayStatics::SaveGameToSlot(SavedSettings, "SettingsSlot", 0);
}

void UAudioSettingsUI::SetSavedSettings() const
{
	if (MasterVolumeSlider) ApplyMasterVolumeValue(MasterVolumeSlider->GetSavedValue());
	if (SfxVolumeSlider) ApplySfxVolumeValue(SfxVolumeSlider->GetSavedValue());
	if (MusicVolumeSlider) ApplyMusicVolumeValue(MusicVolumeSlider->GetSavedValue());
	if (HorizontalSensitivitySlider) ApplyHorizontalSensitivityValue(HorizontalSensitivitySlider->GetSavedValue());
	if (VerticalSensitivitySlider) ApplyVerticalSensitivityValue(VerticalSensitivitySlider->GetSavedValue());
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

void UAudioSettingsUI::ApplyHorizontalSensitivityValue(const float InValue) const
{
	GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->SetMouseHorizontalSensitivity(InValue);
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetMouseHorizontalSensitivity(InValue);
	if (HorizontalSensitivitySlider) HorizontalSensitivitySlider->SetValue(InValue);
}

void UAudioSettingsUI::ApplyVerticalSensitivityValue(const float InValue) const
{
	GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->SetMouseVerticalSensitivity(InValue);
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetMouseVerticalSensitivity(InValue);
	if (VerticalSensitivitySlider) VerticalSensitivitySlider->SetValue(InValue);
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
