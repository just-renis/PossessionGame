#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AudioSettingsUI.generated.h"

class USliderUI;

UCLASS()
class POSSESSIONGAME_API UAudioSettingsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateDefaultAudioSettings();
	void ApplySettings();
	void SetSavedSettings();
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USliderUI> MasterVolumeSlider;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USliderUI> MusicVolumeSlider;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USliderUI> SfxVolumeSlider;

	UFUNCTION()
	void ApplyMasterVolumeValue(float Value);
	UFUNCTION()
	void ApplySfxVolumeValue(float Value);
	UFUNCTION()
	void ApplyMusicVolumeValue(float Value);
};
