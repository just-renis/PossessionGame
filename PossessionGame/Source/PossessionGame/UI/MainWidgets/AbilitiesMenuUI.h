#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitiesMenuUI.generated.h"

class UMainMenuButtonUI;
enum class ECharacterType : uint8;
class UVariationCharacterIconUI;
class UAbilityIconUI;
class UCustomTextUI;
class UConfirmationPopupUI;
class UTextBlock;

USTRUCT()
struct FAbilitiesContainerData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditDefaultsOnly)
	FText Details;
};

USTRUCT()
struct FAbilitiesContainer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FAbilitiesContainerData> Container;

	FAbilitiesContainerData GetData(const int32 InIndex) const
	{
		if (Container.IsValidIndex(InIndex)) return Container[InIndex];
		return FAbilitiesContainerData();
	}
};

UCLASS()
class POSSESSIONGAME_API UAbilitiesMenuUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void ToggleDetailsPanel(const bool bState, const FText& InText) const;
	void OpenUI();
	void CloseUI();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Abilities Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> LockedTexture;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities Data", meta = (AllowPrivateAccess = true))
	TMap<ECharacterType, FAbilitiesContainer> AbilitiesData;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillPointsText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UConfirmationPopupUI> UnlockAbilityPopup;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityIconUI> FirstAbility;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityIconUI> SecondAbility;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityIconUI> ThirdAbility;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityIconUI> FourthAbility;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> GuardianCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> RangerCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVariationCharacterIconUI> TricksterCharacterIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCustomTextUI> DetailsPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMainMenuButtonUI> ConfirmButton;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnim;
	UPROPERTY()
	TObjectPtr<UVariationCharacterIconUI> CurrentlySelectedIcon;
	UPROPERTY()
	FWidgetAnimationDynamicEvent FadeOutAnimEndDelegate;
	UPROPERTY()
	TArray<TObjectPtr<UAbilityIconUI>> Abilities;

	virtual void NativeConstruct() override;
	void Toggle(const bool bState);
	void SwitchCharacterAbilities(const ECharacterType InCharacterName);
	void TriggerSwitchOnTheFirstOpen();
	UFUNCTION()
	void FadeOutAnimFinished();
};
