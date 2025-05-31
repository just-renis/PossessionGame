#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationsUI.generated.h"

enum class EVariation : uint8;
enum class EVariationUnlock : uint8;
class USaveSlot;
class UInvalidationBox;
class UVariationOptionUI;
class UVariationUnlockDetailsUI;
class UWidgetSwitcher;
class UVariationCharacterIconUI;

UCLASS()
class POSSESSIONGAME_API UVariationsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	void CloseUI();
private:
	bool bHasAlreadyOpened = false;
	int32 CurrentCharacterSelected = 0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> GuardianCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> RangerCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> TricksterCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> VariationsSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationUnlockDetailsUI> UnlockDetailsPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> GuardianVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> GuardianVariationGold;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> RangerVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> RangerVariationBlue;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TricksterVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TricksterVariationPurple;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> VariationMenuFadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DetailPanelFadeInAnim;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;
	UPROPERTY()
	FWidgetAnimationDynamicEvent DetailsPanelFadeOutDelegate;

	FString GetUnlockProgressString(const TObjectPtr<USaveSlot>& InSaveSlot, const EVariationUnlock InVariationUnlockName);

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void UpdateVariationUnlocksProgress();
	void SetSelectedVariation(const EVariation InVariation);
	UFUNCTION()
	void SwitchVariation(const int32 InIndex);
	UFUNCTION()
	void ToggleDetailsPanel(const bool bState, const FString& InDetailsText);
	UFUNCTION()
	void HideDetailsPanel();
	UFUNCTION()
	void HideVariationsPanel();
};
