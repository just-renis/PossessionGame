#include "AudioSettingsUI.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/SliderUI.h"
#include "PossessionGame/Utilities/Global Utilities/SettingsSave.h"

void UAudioSettingsUI::UpdateDefaultAudioSettings()
{
	TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0));
	ApplyMasterVolumeValue(SavedSettings->GetMasterVolumeValue());
	ApplySfxVolumeValue(SavedSettings->GetSfxVolumeValue());
	ApplyMusicVolumeValue(SavedSettings->GetMusicVolumeValue());
}

void UAudioSettingsUI::ApplySettings()
{
	TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0));
	MasterVolumeSlider->SaveValue();
	SavedSettings->SetMasterVolume(MasterVolumeSlider->GetSavedValue());
	SfxVolumeSlider->SaveValue();
	SavedSettings->SetSfxVolume(GetWorld(), SfxVolumeSlider->GetSavedValue());
	MusicVolumeSlider->SaveValue();
	SavedSettings->SetMusicVolume(GetWorld(), MusicVolumeSlider->GetSavedValue());
	UGameplayStatics::SaveGameToSlot(SavedSettings, "SettingsSlot", 0);
}

void UAudioSettingsUI::SetSavedSettings()
{
	ApplyMasterVolumeValue(MasterVolumeSlider->GetSavedValue());
	ApplySfxVolumeValue(SfxVolumeSlider->GetSavedValue());
	ApplyMusicVolumeValue(MusicVolumeSlider->GetSavedValue());
}

void UAudioSettingsUI::ApplyMasterVolumeValue(float Value)
{
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetMasterVolume(Value);
	MasterVolumeSlider->SetValue(Value);
}

void UAudioSettingsUI::ApplySfxVolumeValue(float Value)
{
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetSfxVolume(GetWorld(), Value);
	SfxVolumeSlider->SetSliderValue(Value);
}

void UAudioSettingsUI::ApplyMusicVolumeValue(float Value)
{
	Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0))->SetMusicVolume(GetWorld(), Value);
	MusicVolumeSlider->SetSliderValue(Value);
}