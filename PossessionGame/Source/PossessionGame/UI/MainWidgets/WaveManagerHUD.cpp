#include "WaveManagerHUD.h"

#include "Animation/WidgetAnimation.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

float UWaveManagerHUD::GetWaveAnnouncementEndTime() const { return WaveCompletedAnnouncementAnim ? WaveCompletedAnnouncementAnim->GetEndTime() : 0.1f; }

void UWaveManagerHUD::UpdateCountdown(const int32 InCountdown)
{
	if (!CountdownText) return;
	if (InCountdown <= 0) CountdownText->SetText(FText::FromString(TEXT("FIGHT!")));
	else CountdownText->SetText(FText::FromString(FString::Format(TEXT("{0}"), { InCountdown })));
}

void UWaveManagerHUD::UpdateEnemiesAmount(const int32 InEnemiesLeft)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnEnemiesAmountDecrease.Broadcast(InEnemiesLeft);
	if (EnemiesLeftText) EnemiesLeftText->SetText(FText::FromString(FString::Format(TEXT("Enemies left: {0}"), { InEnemiesLeft })));
}

void UWaveManagerHUD::UpdateCurrentWaveText(const int32 InCurrentWave)
{
	if (CurrentWaveText) CurrentWaveText->SetText(FText::FromString(FString::Format(TEXT("Wave {0}"), { InCurrentWave })));
}

void UWaveManagerHUD::ToggleCountdownText()
{
	if (CountdownText)
	{
		if (CountdownText->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) CountdownText->SetVisibility(ESlateVisibility::Collapsed);
		else CountdownText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWaveManagerHUD::ToggleEnemiesLeftText()
{
	if (EnemiesLeftText)
	{
		if (EnemiesLeftText->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) EnemiesLeftText->SetVisibility(ESlateVisibility::Collapsed);
		else EnemiesLeftText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWaveManagerHUD::ToggleCurrentWaveText()
{
	if (CurrentWaveText)
	{
		if (CurrentWaveText->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) CurrentWaveText->SetVisibility(ESlateVisibility::Collapsed);
		else CurrentWaveText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWaveManagerHUD::PlayWavePanelFadeOutAnim() { if (WavePanelFadeOutAnim) PlayAnimation(WavePanelFadeOutAnim); }

void UWaveManagerHUD::PlayWaveCompletedAnim()
{
	if (AnnouncementInv) AnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (WaveCompletedAnnouncementAnim)
	{
		PlayAnimation(WaveCompletedAnnouncementAnim);
		UKismetSystemLibrary::K2_SetTimer(this, "HideWaveCompletedAnnouncement", WaveCompletedAnnouncementAnim->GetEndTime(), false);
	}
}

void UWaveManagerHUD::PlayCountdownAnim() { if (CountdownAnnouncementAnim) PlayAnimation(CountdownAnnouncementAnim); }

void UWaveManagerHUD::HideWaveCompletedAnnouncement() const { if (AnnouncementInv) AnnouncementInv->SetVisibility(ESlateVisibility::Collapsed); }
