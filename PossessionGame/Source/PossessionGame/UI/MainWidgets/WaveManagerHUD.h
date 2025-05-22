#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaveManagerHUD.generated.h"

enum class EGameMode : uint8;
class UProgressBar;
class UInvalidationBox;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UWaveManagerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	bool GetIsObjectivePanelFadeInAtLeastOnce() const { return bIsObjectivePanelFadeInAtLeastOnce; }
	float GetWaveAnnouncementEndTime() const;
	float GetObjectivePanelFadeInEndTime() const;
	
	void UpdateCountdown(const int32 InCountdown);
	void ToggleCountdownText();
	void PlayObjectivePanelFadeOutAnim();
	void PlayObjectivePanelFadeInAnim();
	void PlayAnnouncementAnim(const FString& InAnnouncementText);
	void PlayCountdownAnim();
	void UpdateTaskProgress(const float InProgress, const float InMaxProgress) const;
	void ToggleTaskProgress(const bool bState, const EGameMode InGameMode) const;
	void UpdateObjective(const EGameMode InGameMode, const int32 InData) const;
	void UpdateObjectiveProgress(const int32 InObjectiveProgress, const int32 InObjective);
private:
	bool bIsObjectivePanelFadeInAtLeastOnce = false;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountdownText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectiveProgressText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectiveText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectiveTitleText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TaskText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TaskProgressPercentageText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AnnouncementText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> TaskProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> TaskProgressInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> AnnouncementInv;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ObjectivePanelFadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> AnnouncementAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CountdownAnnouncementAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ObjectiveCompletedFadeInAnim;

	UFUNCTION()
	void HideWaveCompletedAnnouncement() const;
	UFUNCTION()
	void PlayObjectiveCompletedAnimFadeOut();
};