#include "AudioSettingsUI.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/SliderUI.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/SettingsSave.h"

void UAudioSettingsUI::UpdateDefaultAudioSettings() const
{
	const TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot("SettingsSlot", 0));
	ApplyMasterVolumeValue(SavedSettings->GetMasterVolumeValue());
	ApplySfxVolumeValue(SavedSettings->GetSfxVolumeValue());
	ApplyMusicVolumeValue(SavedSettings->GetMusicVolumeValue());
}

void UAudioSettingsUI::ApplySettings() const
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

void UAudioSettingsUI::SetSavedSettings() const
{
	if (MasterVolumeSlider) ApplyMasterVolumeValue(MasterVolumeSlider->GetSavedValue());
	if (SfxVolumeSlider) ApplySfxVolumeValue(SfxVolumeSlider->GetSavedValue());
	if (MusicVolumeSlider) ApplyMusicVolumeValue(MusicVolumeSlider->GetSavedValue());
}

void UAudioSettingsUI::NativeConstruct()
{
	Super::NativeConstruct();
	#if WITH_EDITOR
		if (ResetSaveSlotButton)
		{
			UE_LOG(LogTemp, Error, TEXT("BUTTON WORKS"));
			ResetSaveSlotButton->OnClicked.AddDynamic(this, &UAudioSettingsUI::ResetSaveSlot);
			ResetSaveSlotButton->SetVisibility(ESlateVisibility::Visible);
		}
	#endif
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

void UAudioSettingsUI::ResetSaveSlot() { if (GetGameInstance()) Cast<UPossessionGameInstance>(GetGameInstance())->ResetSaveSlot(); }
