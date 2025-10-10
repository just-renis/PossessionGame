#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "AbilitiesMenuUI.generated.h"

class UButton;
class UCloseButtonUI;
class UWidgetSwitcher;
class UCharacterAbilitiesUI;
class UAbilityResetButtonUI;
class UVariationCharacterIconUI;
class UCustomTextUI;
class UConfirmationPopupUI;
class UTextBlock;

USTRUCT()
struct FCurrentlySelectedCharacter
{
	GENERATED_BODY()

	ECharacterType GetCharacterType() const { return Character; }
	TObjectPtr<UVariationCharacterIconUI> GetIcon() { return Icon; }
	void SetCurrentlySelectedCharacter(const ECharacterType InCharacter, const TObjectPtr<UVariationCharacterIconUI>& InIcon)
	{
		Character = InCharacter;
		Icon = InIcon;
	}
private:
	ECharacterType Character = ECharacterType::Random;
	UPROPERTY()
	TObjectPtr<UVariationCharacterIconUI> Icon;
};

UCLASS()
class POSSESSIONGAME_API UAbilitiesMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	TObjectPtr<UTexture2D> GetAbilityIcon(const ECharacterType InCharacter, const int32 InAbilityIndex) const;
	
	void ToggleDetailsPanel(const bool bState, const FText& InText) const;
	void OpenUI();
	UFUNCTION()
	void CloseUI();
private:
	bool FirstTimeOpenInstance = true;
	bool bIsAbilityUnlockPopupOpened = false;
	bool bWaitingForSwitch = false;
	float SwitchDelayAccumulator = 0.f;
	int32 AbilityToUnlock;
	int32 ActiveWidgetIndex = 0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillPointsText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UConfirmationPopupUI> UnlockAbilityPopup;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> CharacterAbilitiesSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterAbilitiesUI> GuardianCharacterAbilities;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterAbilitiesUI> RangerCharacterAbilities;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterAbilitiesUI> TricksterCharacterAbilities;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterAbilitiesUI> BreacherCharacterAbilities;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterAbilitiesUI> WraithCharacterAbilities;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterAbilitiesUI> TankCharacterAbilities;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterAbilitiesUI> SpyCharacterAbilities;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterAbilitiesUI> SamuraiCharacterAbilities;
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
	TObjectPtr<UWidgetSwitcher> CharactersSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LeftArrowButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RightArrowButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityResetButtonUI> FirstAbilityResetButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityResetButtonUI> SecondAbilityResetButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCustomTextUI> DetailsPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCloseButtonUI> CloseButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CharacterTransitionAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ArrowLeftHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ArrowLeftPressAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ArrowRightHoverAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ArrowRightPressAnim;
	UPROPERTY()
	FCurrentlySelectedCharacter CurrentlySelectedCharacter;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutAnimEndDelegate;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void Toggle(const bool bState);
	void SwitchCharacterAbilities(const ECharacterType InCharacterName);
	void TriggerSwitchOnTheFirstOpen();
	void SetupButtons();
	void OpenUnlockAbilityPopup(const int32 InAbilityIndex);
	void SetupUnlockAbilityPopup();
	void UpdateSkillPointsText() const;
	void UpdateAbilityIcon() const;
	void SwitchAbilityIcons() const;
	UFUNCTION()
	void FadeOutAnimFinished();
	UFUNCTION()
	void FirstSlotResetClicked();
	UFUNCTION()
	void SecondSlotResetClicked();
	UFUNCTION()
	void UnlockAbility();
	UFUNCTION()
	void FadeOutUnlockAbilityPopup();
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
