#include "SettingsSave.h"
#include "Kismet/GameplayStatics.h"

void USettingsSave::SetMusicVolume(TObjectPtr<UWorld> WorldRef, float InMusicVolumeValue)
{
	MusicVolumeValue = InMusicVolumeValue;
	UGameplayStatics::SetSoundMixClassOverride(WorldRef, MasterSoundMix, MusicSoundClass, MusicVolumeValue * MasterVolumeValue, 1.f, 0.f);
	UGameplayStatics::PushSoundMixModifier(WorldRef, MasterSoundMix);
}

void USettingsSave::SetSfxVolume(TObjectPtr<UWorld> WorldRef, float InSfxVolumeValue)
{
	SfxVolumeValue = InSfxVolumeValue;
	UGameplayStatics::SetSoundMixClassOverride(WorldRef, MasterSoundMix, SfxSoundClass, SfxVolumeValue * MasterVolumeValue, 1.f, 0.f);
	UGameplayStatics::PushSoundMixModifier(WorldRef, MasterSoundMix);
}

void USettingsSave::LoadSettings(TObjectPtr<UWorld> WorldRef)
{
	SetMusicVolume(WorldRef, MusicVolumeValue);
	SetSfxVolume(WorldRef, SfxVolumeValue);
	FInternationalization::Get().SetCurrentLanguage(Language);
}
