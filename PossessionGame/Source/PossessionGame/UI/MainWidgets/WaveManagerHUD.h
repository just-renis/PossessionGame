#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaveManagerHUD.generated.h"

class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UWaveManagerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateCountdown(int32 Countdown);
	void UpdateEnemiesAmount(int32 EnemiesLeft);
	void UpdateCurrentWaveText(int32 CurrentWave);
	void ToggleCountdownText();
	void ToggleEnemiesLeftText();
	void ToggleCurrentWaveText();
	void PlayWavePanelFadeOutAnim();
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountdownText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EnemiesLeftText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentWaveText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> WavePanelFadeOutAnim;
};