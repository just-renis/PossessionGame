#include "WaveManagerHUD.h"
#include "Animation/WidgetAnimation.h"
#include "Components/InvalidationBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Enums/EGameMode.h"

float UWaveManagerHUD::GetWaveAnnouncementEndTime() const { return AnnouncementAnim ? AnnouncementAnim->GetEndTime() : 0.1f; }

float UWaveManagerHUD::GetObjectivePanelFadeOutEndTime() const { return ObjectivePanelFadeOutAnim ? ObjectivePanelFadeOutAnim->GetEndTime() : 0.1f; }

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

void UWaveManagerHUD::PlayObjectivePanelFadeOutAnim()
{
	if (!ObjectivePanelFadeOutAnim) return;
	PlayAnimation(ObjectivePanelFadeOutAnim);
}

void UWaveManagerHUD::PlayObjectivePanelFadeInAnim()
{
	if (!ObjectivePanelFadeOutAnim) return;
	bIsObjectivePanelFadeInAtLeastOnce = true;
	PlayAnimation(ObjectivePanelFadeOutAnim, 0.f, 1, EUMGSequencePlayMode::Reverse);
}

void UWaveManagerHUD::PlayAnnouncementAnim(const FString& InAnnouncementText)
{
	if (AnnouncementText) AnnouncementText->SetText(FText::FromString(InAnnouncementText));
	if (AnnouncementInv) AnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (AnnouncementAnim)
	{
		PlayAnimation(AnnouncementAnim);
		UKismetSystemLibrary::K2_SetTimer(this, "HideWaveCompletedAnnouncement", AnnouncementAnim->GetEndTime(), false);
	}
}

void UWaveManagerHUD::PlayCountdownAnim() { if (CountdownAnnouncementAnim) PlayAnimation(CountdownAnnouncementAnim); }

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
	if (!ObjectiveText) return;
	switch (InGameMode)
	{
		case EGameMode::WaveMode: ObjectiveText->SetText(FText::FromString("Defeat " + FString::FromInt(InData) + " waves")); break;
		case EGameMode::HackTerminalsMode: ObjectiveText->SetText(FText::FromString("Hack " + FString::FromInt(InData) + " terminals")); break;
		case EGameMode::ChargeTerminalMode: ObjectiveText->SetText(FText::FromString("Charge the terminal")); break;
		case EGameMode::BossMode: ObjectiveText->SetText(FText::FromString("Kill the boss")); break;
		default: ;
	}
}

void UWaveManagerHUD::UpdateObjectiveProgress(const int32 InObjectiveProgress, const int32 InObjective) const
{
	if (!ObjectiveProgressText) return;
	ObjectiveProgressText->SetText(FText::FromString(FString::FromInt(InObjectiveProgress) + " / " + FString::FromInt(InObjective)));
}

void UWaveManagerHUD::HideWaveCompletedAnnouncement() const { if (AnnouncementInv) AnnouncementInv->SetVisibility(ESlateVisibility::Collapsed); }
