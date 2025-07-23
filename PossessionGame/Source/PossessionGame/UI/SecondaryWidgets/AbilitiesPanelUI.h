#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitiesPanelUI.generated.h"

class UTextBlock;
class UImage;
class UAbility;
enum class EUIBar : uint8;

UCLASS()
class POSSESSIONGAME_API UAbilitiesPanelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetOverlaysVisibility(const bool bState) const;
	void CollapseAbilityIcon(const int32 Index) const;
	void UpdateAbilityUI(const TObjectPtr<UAbility>& InAbility);
	void SetAsCurrent();
	void SetAbilityIcon(const int32 InAbilityIndex, const int32 InAbilityIconIndex) const;
private:
	bool bFirstAbilityFullyCharged = false;
	bool bSecondAbilityFullyCharged = false;
	UPROPERTY(EditAnywhere, Category = "Ability Data", meta = (AllowPrivateAccess = true))
	TMap<int32, TObjectPtr<UTexture2D>> AbilitiesIcons;
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
	TObjectPtr<UWidget> FirstAbilityOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> SecondAbilityOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FirstChargesText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SecondChargesText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FirstKeyBindText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SecondKeyBindText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Ability1UseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Ability2UseAnim;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_FirstAbilityRadial;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_SecondAbilityRadial;
	UPROPERTY()
	FTimerHandle FirstAbilityHandle;
	UPROPERTY()
	FTimerHandle SecondAbilityHandle;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	void SetAbilitySlot(const int32 InAbilityIndex) const;
	void PlayAbilityUseAnim(const int32 InAbilityIndex, const bool bIsCharged, const bool bPlayReverseAfterwards);
	void PlayAbilityAnim(const int32 InAbilityIndex, const bool bState);
	void UpdateAbilityKeyBindText(const FName& InKeyBindName, const FText& InKeyName) const;
	void PlayAbilityEndAnim(const int32 InAbilityIndex, const bool bIsCharged);
};
