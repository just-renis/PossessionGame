#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationsUI.generated.h"

class UVariationCurrencyPanelUI;
enum class EVariation : uint8;
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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> GuardianCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> RangerCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> TricksterCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> VariationsSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationUnlockDetailsUI> VariationDetailsPanel;
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
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<EVariation, int32> UnlockPrices;
	UPROPERTY()
	TObjectPtr<USaveSlot> SaveSlot;
	UPROPERTY()
	TObjectPtr<UVariationCharacterIconUI> CurrentlySelectedIcon;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void SetSelectedVariation(const EVariation InVariation);
	void SetButtonStatesOnToggle(const bool bState) const;
	UFUNCTION()
	void SwitchVariation(const int32 InIndex);
	UFUNCTION()
	void ToggleDetailsPanel(const bool bState, const EVariation InVariationName, const FString& InVariationDetailsText, const FString& InUnlockDetailsText);
	UFUNCTION()
	void HideVariationsPanel();
};
