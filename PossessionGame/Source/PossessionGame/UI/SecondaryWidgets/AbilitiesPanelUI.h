#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitiesPanelUI.generated.h"

class UTextBlock;
class UImage;
class UAbility;
enum class EUIBar : uint8;
class UOverlay;

UCLASS()
class POSSESSIONGAME_API UAbilitiesPanelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetOverlaysVisibility(const bool bState) const;
	void CollapseAbilityIcon(const int32 Index) const;
	void UpdateAbilityUI(const TObjectPtr<UAbility>& InAbility);
	void SetAsCurrent();
private:
	bool bFirstAbilityFullyCharged = false;
	bool bSecondAbilityFullyCharged = false;
	bool bThirdAbilityFullyCharged = false;
	UPROPERTY(EditAnywhere, Category = "Ability Icons", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> AbilityIcon1;
	UPROPERTY(EditAnywhere, Category = "Ability Icons", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> AbilityIcon2;
	UPROPERTY(EditAnywhere, Category = "Ability Icons", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> AbilityIcon3;
	UPROPERTY(EditDefaultsOnly, Category = "Material", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_RadialProgress;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FirstAbilityProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FirstAbilityImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SecondAbilityProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SecondAbilityImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ThirdAbilityProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ThirdAbilityImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> FirstAbilityOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> SecondAbilityOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> ThirdAbilityOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FirstChargesText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SecondChargesText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ThirdChargesText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FirstKeyBindText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SecondKeyBindText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ThirdKeyBindText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Ability1UseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Ability2UseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Ability3UseAnim;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_FirstAbilityRadial;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_SecondAbilityRadial;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_ThirdAbilityRadial;
	UPROPERTY()
	FTimerHandle FirstAbilityHandle;
	UPROPERTY()
	FTimerHandle SecondAbilityHandle;
	UPROPERTY()
	FTimerHandle ThirdAbilityHandle;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	void SetAbilitySlot(const int32 InAbilityIndex, const TObjectPtr<UTexture2D>& InIcon, const TObjectPtr<UImage>& InImage, const TObjectPtr<UOverlay>& InOverlay) const;
	void PlayAbilityUseAnim(const int32 InAbilityIndex, const bool bIsCharged, const bool bPlayReverseAfterwards);
	void PlayAbilityAnim(const int32 InAbilityIndex, const bool bState);
	void UpdateAbilityKeyBindText(const FName& InKeyBindName, const FText& InKeyName) const;
	void PlayAbilityEndAnim(const int32 InAbilityIndex, const bool bIsCharged);
};
