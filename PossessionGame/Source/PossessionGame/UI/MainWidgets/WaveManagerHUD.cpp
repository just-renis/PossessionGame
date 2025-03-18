#include "WaveManagerHUD.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UWaveManagerHUD::UpdateCountdown(int32 Countdown)
{
	if (CountdownText) CountdownText->SetText(FText::FromString(FString::Format(TEXT("Wave starts in: {0}"), { Countdown })));
}

void UWaveManagerHUD::UpdateEnemiesAmount(int32 EnemiesLeft)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnEnemiesAmountDecrease.Broadcast(EnemiesLeft);
	if (EnemiesLeftText) EnemiesLeftText->SetText(FText::FromString(FString::Format(TEXT("Enemies left: {0}"), { EnemiesLeft })));
}

void UWaveManagerHUD::UpdateCurrentWaveText(int32 CurrentWave)
{
	if (CurrentWaveText) CurrentWaveText->SetText(FText::FromString(FString::Format(TEXT("Wave {0}"), { CurrentWave })));
}

void UWaveManagerHUD::ToggleCountdownText()
{
	if (CountdownText)
	{
		if (CountdownText->GetVisibility() == ESlateVisibility::Visible) CountdownText->SetVisibility(ESlateVisibility::Collapsed);
		else CountdownText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UWaveManagerHUD::ToggleEnemiesLeftText()
{
	if (EnemiesLeftText)
	{
		if (EnemiesLeftText->GetVisibility() == ESlateVisibility::Visible) EnemiesLeftText->SetVisibility(ESlateVisibility::Collapsed);
		else EnemiesLeftText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UWaveManagerHUD::ToggleCurrentWaveText()
{
	if (CurrentWaveText)
	{
		if (CurrentWaveText->GetVisibility() == ESlateVisibility::Visible) CurrentWaveText->SetVisibility(ESlateVisibility::Collapsed);
		else CurrentWaveText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UWaveManagerHUD::PlayWavePanelFadeOutAnim() { if (WavePanelFadeOutAnim) PlayAnimation(WavePanelFadeOutAnim); }
