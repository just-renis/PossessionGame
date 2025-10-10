#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationsUI.generated.h"

class UButton;
enum class ECharacterType : uint8;
class UVariationCurrencyPanelUI;
enum class EVariation : uint8;
class USaveSlot;
class UInvalidationBox;
class UVariationOptionUI;
class UVariationUnlockDetailsUI;
class UWidgetSwitcher;
class UVariationCharacterIconUI;
class UCloseButtonUI;

UCLASS()
class POSSESSIONGAME_API UVariationsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void OpenUI();
	UFUNCTION()
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
	TObjectPtr<UVariationCharacterIconUI> BreacherCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> WraithCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> TankCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> SpyCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> SamuraiCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> VariationsSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> CharactersSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationUnlockDetailsUI> VariationDetailsPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationUnlockDetailsUI> UnlockDetailsPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RightArrowButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LeftArrowButton;
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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> BreacherVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> WraithVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TankVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> SpyVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> SamuraiVariationDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCloseButtonUI> CloseButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> VariationMenuFadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ArrowLeftHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ArrowLeftPressAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ArrowRightHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ArrowRightPressAnim;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<EVariation, int32> UnlockPrices;
	UPROPERTY()
	TObjectPtr<USaveSlot> SaveSlot;
	UPROPERTY()
	TObjectPtr<UVariationCharacterIconUI> CurrentlySelectedIcon;
	UPROPERTY()
	TObjectPtr<UVariationCurrencyPanelUI> VariationCurrencyPanel;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void SetSelectedVariation(const EVariation InVariation);
	UFUNCTION()
	void SwitchVariation(const ECharacterType InCharacterName);
	UFUNCTION()
	void ToggleDetailsPanel(const bool bState, const EVariation InVariationName, const FText& InVariationDetailsText, const FText& InUnlockDetailsText);
	UFUNCTION()
	void HideVariationsPanel();
	UFUNCTION()
	void SwitchCharactersSelectionRight();
	UFUNCTION()
	void SwitchCharactersSelectionLeft();
	UFUNCTION()
	void LeftArrowButtonHovered();
	UFUNCTION()
	void LeftArrowButtonUnHovered();
	UFUNCTION()
	void RightArrowButtonHovered();
	UFUNCTION()
	void RightArrowButtonUnHovered();
};
