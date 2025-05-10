#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitiesPanelUI.generated.h"

enum class EUIBar : uint8;
class UOverlay;
class UImage;

UCLASS()
class POSSESSIONGAME_API UAbilitiesPanelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetOverlaysVisibility(const bool bState) const;
	void CollapseAbilityIcon(const int32 Index) const;
	void TriggerAbilityIconReady(const EUIBar InBar, const float InCurrent, const float InMax) const;
	void SetAsCurrent();
private:
	UPROPERTY(EditAnywhere, Category = "Ability Icons", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> AbilityIcon1;
	UPROPERTY(EditAnywhere, Category = "Ability Icons", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> AbilityIcon2;
	UPROPERTY(EditAnywhere, Category = "Ability Icons", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> AbilityIcon3;
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
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Ability1UseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Ability2UseAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Ability3UseAnim;
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> MI_AbilityRadials;
	UPROPERTY()
	TArray<FTimerHandle> TimerHandles;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void SetAbilitySlot(const TObjectPtr<UTexture2D>& InIcon, const TObjectPtr<UImage>& InImage, const TObjectPtr<UOverlay>& InOverlay);
	void PlayAbilityUseAnim(const int32 InAbilityIndex, const bool bPlayReverseAfterwards);
	void PlayAbilityAnim(const int32 InAbilityIndex, const bool bState);
	UFUNCTION()
	void PlayAbilityEndAnim(const int32 InAbilityIndex);
};
