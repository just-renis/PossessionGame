#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VariationsUI.generated.h"

class UArrowButtonUI;
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
	TObjectPtr<UVariationCharacterIconUI> SpyCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> TankCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> SamuraiCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> VariationsSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> CharactersSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> RangerVariations;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> TricksterVariations;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> GuardianVariations;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WraithVariations;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> BreacherVariations;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> TankVariations;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> SpyVariations;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> SamuraiVariations;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationUnlockDetailsUI> VariationDetailsPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationUnlockDetailsUI> UnlockDetailsPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UArrowButtonUI> RightArrowButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UArrowButtonUI> LeftArrowButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UArrowButtonUI> VariationPageRightArrowButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UArrowButtonUI> VariationPageLeftArrowButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> GuardianDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> GuardianGreen;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> GuardianPurple;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> GuardianBlack;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> RangerDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> RangerBlue;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> RangerGreen;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> RangerBlack;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TricksterDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TricksterPurple;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TricksterLime;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TricksterBlack;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> BreacherDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> BreacherRed;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> BreacherGreen;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> BreacherBlack;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> WraithDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> WraithPurple;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> WraithBlue;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> WraithBlack;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TankDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TankOrange;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TankGreen;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> TankBlack;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> SpyDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> SpyOrange;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> SpyYellow;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> SpyBlack;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> SamuraiDefault;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> SamuraiRed;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> SamuraiBlue;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationOptionUI> SamuraiBlack;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCloseButtonUI> CloseButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> VariationMenuFadeInAnim;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<EVariation, int32> UnlockPrices;
	UPROPERTY()
	TObjectPtr<USaveSlot> SaveSlot;
	UPROPERTY()
	TObjectPtr<UVariationCharacterIconUI> CurrentlySelectedIcon;
	UPROPERTY()
	TObjectPtr<UVariationCurrencyPanelUI> VariationCurrencyPanel;
	UPROPERTY()
	TObjectPtr<UWidgetSwitcher> CurrentVariationsSwitcherSelected;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutDelegate;
	UPROPERTY()
	TMap<EVariation, TObjectPtr<UVariationOptionUI>> VariationOptions;

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
	void SwitchVariationPageRight();
	UFUNCTION()
	void SwitchVariationPageLeft();
};
