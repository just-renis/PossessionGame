#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SettingsSave.generated.h"

UCLASS()
class POSSESSIONGAME_API USettingsSave : public USaveGame
{
	GENERATED_BODY()
public:
	float GetMasterVolumeValue() { return MasterVolumeValue; }
	float GetSfxVolumeValue() { return SfxVolumeValue; }
	float GetMusicVolumeValue() { return MusicVolumeValue; }
	
	void SetMasterVolume(float InMasterVolumeValue) { MasterVolumeValue = InMasterVolumeValue; }
	void SetMusicVolume(TObjectPtr<UWorld> WorldRef, float InMusicVolumeValue);
	void SetSfxVolume(TObjectPtr<UWorld> WorldRef, float InSfxVolumeValue);
	void LoadSettings(TObjectPtr<UWorld> WorldRef);
	void LoadSettingsOnFirstLaunch(TObjectPtr<UWorld> WorldRef);
private:
	UPROPERTY()
	float MasterVolumeValue;
	UPROPERTY()
	float MusicVolumeValue;
	UPROPERTY()
	float SfxVolumeValue;
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundClass> MusicSoundClass;
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundClass> SfxSoundClass;
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundMix> MasterSoundMix;
};
