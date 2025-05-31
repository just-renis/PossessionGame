#include "WaveManagerHUD.h"
#include "Animation/WidgetAnimation.h"
#include "Components/InvalidationBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Enums/EGameMode.h"

float UWaveManagerHUD::GetWaveAnnouncementEndTime() const { return AnnouncementAnim->GetEndTime(); }

float UWaveManagerHUD::GetObjectivePanelFadeInEndTime() const { return ObjectivePanelFadeInAnim->GetEndTime(); }

void UWaveManagerHUD::UpdateCountdown(const int32 InCountdown)
{
	if (!CountdownText) return;
	if (InCountdown <= 0) CountdownText->SetText(FText::FromString(TEXT("FIGHT!")));
	else CountdownText->SetText(FText::FromString(FString::Format(TEXT("{0}"), { InCountdown })));
}

void UWaveManagerHUD::ToggleCountdownText()
{
	if (CountdownText)
	{
		if (CountdownText->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) CountdownText->SetVisibility(ESlateVisibility::Collapsed);
		else CountdownText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWaveManagerHUD::PlayObjectivePanelFadeInAnim(const bool bState)
{
	PlayAnimation(ObjectivePanelFadeInAnim, 0, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UWaveManagerHUD::PlayAnnouncementAnim(const FString& InAnnouncementText)
{
	if (AnnouncementText) AnnouncementText->SetText(FText::FromString(InAnnouncementText));
	if (AnnouncementInv) AnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimation(AnnouncementAnim);
	GetWorld()->GetTimerManager().SetTimer(AnnouncementHandle, this, &UWaveManagerHUD::HideWaveCompletedAnnouncement, AnnouncementAnim->GetEndTime(), false);
}

void UWaveManagerHUD::PlayCountdownAnim() { PlayAnimation(CountdownAnnouncementAnim); }

void UWaveManagerHUD::UpdateTaskProgress(const float InProgress, const float InMaxProgress) const
{
	if (!TaskProgressBar || !TaskProgressPercentageText) return;
	TaskProgressBar->SetPercent(InProgress / InMaxProgress);
	TaskProgressPercentageText->SetText(FText::FromString(FString::FormatAsNumber(InProgress / InMaxProgress * 100.f) + "%"));
}

void UWaveManagerHUD::ToggleTaskProgress(const bool bState, const EGameMode InGameMode) const
{
	if (!TaskProgressInv || !TaskText) return;
	TaskProgressInv->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	TaskText->SetText(FText::FromString(InGameMode == EGameMode::HackTerminalsMode ? "Terminal Hack Progress" : "Charge Terminal Progress"));
}

void UWaveManagerHUD::UpdateObjective(const EGameMode InGameMode, const int32 InData) const
{
	if (!ObjectiveText || !ObjectiveTitleText) return;
	switch (InGameMode)
	{
		case EGameMode::WaveMode:
			ObjectiveTitleText->SetText(FText::FromString("Objectives 1"));
			ObjectiveText->SetText(FText::FromString("Defeat " + FString::FromInt(InData) + " waves"));
			break;
		case EGameMode::HackTerminalsMode:
			ObjectiveTitleText->SetText(FText::FromString("Objectives 2"));
			ObjectiveText->SetText(FText::FromString("Hack " + FString::FromInt(InData) + " terminals"));
			break;
		case EGameMode::ChargeTerminalMode:
			ObjectiveTitleText->SetText(FText::FromString("Objectives 3"));
			ObjectiveText->SetText(FText::FromString("Charge the terminal"));
			break;
		case EGameMode::BossMode: 
			ObjectiveTitleText->SetText(FText::FromString("Objectives 4"));
			ObjectiveText->SetText(FText::FromString("Kill the boss"));
			break;
		default: ;
	}
}

void UWaveManagerHUD::UpdateObjectiveProgress(const int32 InObjectiveProgress, const int32 InObjective)
{
	if (!ObjectiveProgressText) return;
	ObjectiveProgressText->SetText(FText::FromString(FString::FromInt(InObjectiveProgress) + " / " + FString::FromInt(InObjective)));
	if (InObjectiveProgress != InObjective) return;
	PlayAnimation(ObjectiveCompletedFadeInAnim);
}

void UWaveManagerHUD::HideWaveCompletedAnnouncement() const { if (AnnouncementInv) AnnouncementInv->SetVisibility(ESlateVisibility::Collapsed); }
