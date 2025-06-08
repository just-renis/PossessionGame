#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AudioSettingsUI.generated.h"

class UButton;
class USliderUI;

UCLASS()
class POSSESSIONGAME_API UAudioSettingsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateDefaultAudioSettings() const;
	void ApplySettings() const;
	void SetSavedSettings() const;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderUI> MasterVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderUI> MusicVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USliderUI> SfxVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResetSaveSlotButton;

	virtual void NativeConstruct() override;
	void ApplyMasterVolumeValue(const float InValue) const;
	void ApplySfxVolumeValue(const float InValue) const;
	void ApplyMusicVolumeValue(const float InValue) const;
	UFUNCTION()
	void ResetSaveSlot();
};
