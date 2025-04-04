#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerGhostUI.generated.h"

class UInvalidationBox;
class UDelegateManager;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UPlayerGhostUI : public UUserWidget
{
	GENERATED_BODY()
public:
	bool IsPressAnyKeyShown() const;
	float GetStartupAnimationLength() const;
	
	void PlayStartupAnimation();
private:
	bool bIsShowPressAnyKeyEnabled = false;
	int32 CurrentTutorialStep = -1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> PopupInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> HintInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> PressAnyKeyToContinueInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HintText;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<FString> HintTexts;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> StartupAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HintAppearAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopupAppearAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PressAnyKeyAppearAnim;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	void SetCurrentTutorialStep(const int32 InStep) { CurrentTutorialStep = InStep; }
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void ShowHint(const int32 InIndex);
	void SetHintText(const FString& InText) const;
	void StopShowingHint() const;
	void ShowPressAnyKey(const bool bState);
};
