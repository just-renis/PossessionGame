#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaveManagerHUD.generated.h"

class UInvalidationBox;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UWaveManagerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	float GetWaveAnnouncementEndTime() const;
	
	void UpdateCountdown(const int32 InCountdown);
	void UpdateEnemiesAmount(const int32 InEnemiesLeft);
	void UpdateCurrentWaveText(const int32 InCurrentWave);
	void ToggleCountdownText();
	void ToggleEnemiesLeftText();
	void ToggleCurrentWaveText();
	void PlayWavePanelFadeOutAnim();
	void PlayWaveCompletedAnim();
	void PlayCountdownAnim();
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountdownText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EnemiesLeftText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentWaveText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> AnnouncementInv;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> WavePanelFadeOutAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> WaveCompletedAnnouncementAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CountdownAnnouncementAnim;

	UFUNCTION()
	void HideWaveCompletedAnnouncement() const;
};