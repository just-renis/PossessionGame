#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitiesPanelUI.generated.h"

enum class EUIBar : uint8;
class UOverlay;
class UImage;
class UProgressBar;

UCLASS()
class POSSESSIONGAME_API UAbilitiesPanelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetOverlaysVisibility(const bool bState) const;
	void CollapseAbilityIcon(const int32 Index) const;
	void TriggerAbilityIconReady(const EUIBar InBar, const float InCurrent, const float InMax) const;
private:
	UPROPERTY(EditAnywhere, Category = "Ability Icons", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> AbilityIcon1;
	UPROPERTY(EditAnywhere, Category = "Ability Icons", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> AbilityIcon2;
	UPROPERTY(EditAnywhere, Category = "Ability Icons", meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> AbilityIcon3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> FirstAbilityProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FirstAbilityImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> SecondAbilityProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SecondAbilityImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ThirdAbilityProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ThirdAbilityImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> FirstAbilityOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> SecondAbilityOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> ThirdAbilityOverlay;

	virtual void NativePreConstruct() override;
};
