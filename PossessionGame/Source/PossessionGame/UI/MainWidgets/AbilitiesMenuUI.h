#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "AbilitiesMenuUI.generated.h"

class UAbilityIconUI;
class UWidgetSwitcher;
class UCharacterAbilitiesUI;
class UAbilityResetButtonUI;
class UMainMenuButtonUI;
enum class ECharacterType : uint8;
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
	void CloseUI();
private:
	bool FirstTimeOpenInstance = true;
	int32 AbilityToUnlock;
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
	TObjectPtr<UVariationCharacterIconUI> GuardianCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> RangerCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> TricksterCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityResetButtonUI> FirstAbilityResetButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityResetButtonUI> SecondAbilityResetButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCustomTextUI> DetailsPanel;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	FCurrentlySelectedCharacter CurrentlySelectedCharacter;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutAnimEndDelegate;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void SwitchCharacterAbilities(const ECharacterType InCharacterName);
	void TriggerSwitchOnTheFirstOpen();
	void SetupButtons();
	void OpenUnlockAbilityPopup(const int32 InAbilityIndex);
	void SetupUnlockAbilityPopup();
	void UpdateSkillPointsText() const;
	void UpdateAbilityIcon() const;
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
};
