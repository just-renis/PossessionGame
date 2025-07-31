#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerGhostUI.generated.h"

class UImage;
class UInvalidationBox;
class UDelegateManager;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UPlayerGhostUI : public UUserWidget
{
	GENERATED_BODY()
public:
	bool IsPressKeyShown() const;
	float GetStartupAnimationLength() const;
	int32 GetCurrentHintIndex() const { return CurrentHintIndex; }
	
	void PlayStartupAnimation();
private:
	int32 CurrentHintIndex = 0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> PopupInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> HintInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> PressKeyToContinueInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HintText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PressKeyToContinueText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HintImage;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<FString> HintTexts;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> StartupAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HintAppearAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopupAppearAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PressKeyAppearAnim;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	FText GetKeyBindText(const int32 InIndex) const;

	virtual void NativeConstruct() override;
	void ShowHint();
	void SetHintText(const int32 InIndex) const;
	void StopShowingHint();
	void ShowPressKey(const bool bState);
	void UpdateKeyBindText(const FName& InKeyBindName, const FText& InKeyBindText) const;
	void DestroyThis();
};
