#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModeSelectUI.generated.h"

class UButton;
class UMainMenuButtonUI;

UCLASS()
class POSSESSIONGAME_API UModeSelectUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UButton> GetBackButton() const;
	
	void PlayFadeInAnim(const bool bState);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> StoryModeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ArenaModeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> BackButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	virtual void NativeConstruct() override;
	UFUNCTION()
	void OpenStoryMode();
	UFUNCTION()
	void OpenArenaMode();
	UFUNCTION()
	void FadeOutAnimFinished();
};
