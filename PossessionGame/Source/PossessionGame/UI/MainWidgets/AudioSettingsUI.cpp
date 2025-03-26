#include "AudioSettingsUI.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/SliderUI.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/SettingsSave.h"

void UAudioSettingsUI::UpdateDefaultAudioSettings()
{
	const TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0));
	ApplyMasterVolumeValue(SavedSettings->GetMasterVolumeValue());
	ApplySfxVolumeValue(SavedSettings->GetSfxVolumeValue());
	ApplyMusicVolumeValue(SavedSettings->GetMusicVolumeValue());
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
	UGameplayStatics::SaveGameToSlot(SavedSettings, "SettingsSlot", 0);
}

void UAudioSettingsUI::SetSavedSettings()
{
	if (MasterVolumeSlider) ApplyMasterVolumeValue(MasterVolumeSlider->GetSavedValue());
	if (SfxVolumeSlider) ApplySfxVolumeValue(SfxVolumeSlider->GetSavedValue());
	if (MusicVolumeSlider) ApplyMusicVolumeValue(MusicVolumeSlider->GetSavedValue());
}

void UAudioSettingsUI::ApplyMasterVolumeValue(float Value)
{
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetMasterVolume(Value);
	if (MasterVolumeSlider) MasterVolumeSlider->SetValue(Value);
}

void UAudioSettingsUI::ApplySfxVolumeValue(float Value)
{
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetSfxVolume(GetWorld(), Value);
	if (SfxVolumeSlider) SfxVolumeSlider->SetSliderValue(Value);
}

void UAudioSettingsUI::ApplyMusicVolumeValue(float Value)
{
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetMusicVolume(GetWorld(), Value);
	if (MusicVolumeSlider) MusicVolumeSlider->SetSliderValue(Value);
}