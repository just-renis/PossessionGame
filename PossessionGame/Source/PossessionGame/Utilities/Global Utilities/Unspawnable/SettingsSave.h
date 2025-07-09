#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SettingsSave.generated.h"

UCLASS()
class POSSESSIONGAME_API USettingsSave : public USaveGame
{
	GENERATED_BODY()
public:
	float GetMasterVolumeValue() const { return MasterVolumeValue; }
	float GetSfxVolumeValue() const { return SfxVolumeValue; }
	float GetMusicVolumeValue() const { return MusicVolumeValue; }
	FString GetCurrentLanguage() const { return Language; }
	
	void SetMasterVolume(float InMasterVolumeValue) { MasterVolumeValue = InMasterVolumeValue; }
	void SetMusicVolume(TObjectPtr<UWorld> WorldRef, float InMusicVolumeValue);
	void SetSfxVolume(TObjectPtr<UWorld> WorldRef, float InSfxVolumeValue);
	void LoadSettings(TObjectPtr<UWorld> WorldRef);
	void SetLanguage(const FString& InLanguage) { Language = InLanguage; }
private:
	UPROPERTY()
	float MasterVolumeValue = 1.f;
	UPROPERTY()
	float MusicVolumeValue = 1.f;
	UPROPERTY()
	float SfxVolumeValue = 1.f;
	UPROPERTY()
	FString Language = "en";
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundClass> MusicSoundClass;
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundClass> SfxSoundClass;
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundMix> MasterSoundMix;
};
