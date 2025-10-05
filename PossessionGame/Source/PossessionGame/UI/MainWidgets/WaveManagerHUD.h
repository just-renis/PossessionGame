#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaveManagerHUD.generated.h"

class UDelegateManager;
enum class EGameMode : uint8;
class UProgressBar;
class UInvalidationBox;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UWaveManagerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	float GetWaveAnnouncementEndTime() const;
	
	void UpdateCountdown(const int32 InCountdown);
	void ToggleCountdownText();
	void PlayObjectivePanelFadeInAnim(const bool bState);
	void PlayAnnouncementAnim(const FText& InAnnouncementText);
	void PlayCountdownAnim();
	void UpdateTaskProgress(const float InProgress, const float InMaxProgress) const;
	void ToggleTaskProgress(const bool bState, const EGameMode InGameMode) const;
	void UpdateObjective(const EGameMode InGameMode, const int32 InData) const;
	void UpdateObjectiveProgress(const int32 InObjectiveProgress, const int32 InObjective, const bool bShowUpgradeSelection = false);
	void ToggleFinishOffEnemiesText(const bool bState);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountdownText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectiveProgressText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectiveText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectiveTitleText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FinishOfEnemiesText;
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
	TObjectPtr<UTextBlock> MiniPopupText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> TimerBoxInv;
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
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FinishOffEnemiesFadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> MiniPopupAnim;
	UPROPERTY()
	FTimerHandle AnnouncementHandle;
	UPROPERTY()
	FTimerHandle TimerUpdateHandle;
	UPROPERTY()
	FWidgetAnimationDynamicEvent OnObjectivePanelAnimEnd;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	FString GetConvertedNumber(const int32 InNumber);

	virtual void NativeConstruct() override;
	void HideWaveCompletedAnnouncement() const;
	void TriggerMiniPopup(const FText& InText);
	void UpdateTimerText();
	virtual void NativeDestruct() override;
	UFUNCTION()
	void OpenUpgradeSelection();
};