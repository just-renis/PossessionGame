#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Utilities/Data Utilities/FLoadLevelData.h"
#include "ModeSelectUI.generated.h"

class USaveSlotMenuUI;
class UButton;
class UMainMenuButtonUI;

UCLASS()
class POSSESSIONGAME_API UModeSelectUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UButton> GetBackButton() const;
	
	void PlayFadeInAnim(const bool bState, const FLoadLevelData& InLevelData = FLoadLevelData());
	void SetParentWidget(const TObjectPtr<USaveSlotMenuUI>& InParentWidget) { ParentWidget = InParentWidget; }
private:
	FLoadLevelData PendingLoadLevelData;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> StoryModeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ArenaModeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> BackButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	TObjectPtr<USaveSlotMenuUI> ParentWidget;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;
	UPROPERTY()
	FTimerHandle LoadLevelHandle;

	virtual void NativeConstruct() override;
	void DisablePlayerInput() const;
	void LoadLevel() const;
	UFUNCTION()
	void OpenStoryMode();
	UFUNCTION()
	void OpenArenaMode();
	UFUNCTION()
	void FadeOutAnimFinished();
};
