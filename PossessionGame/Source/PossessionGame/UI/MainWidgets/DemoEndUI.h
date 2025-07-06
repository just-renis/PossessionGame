#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoEndUI.generated.h"

class UMainMenuButtonUI;

UCLASS()
class POSSESSIONGAME_API UDemoEndUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ContinueButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ReturnToMenuButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeInFinishedDelegate;
	UPROPERTY()
	FTimerHandle PauseHandle;

	virtual void NativeConstruct() override;
	void PauseGame() const;
	UFUNCTION()
	void FadeInFinished();
	UFUNCTION()
	void ReturnToMainMenu();
	UFUNCTION()
	void Continue();
};
