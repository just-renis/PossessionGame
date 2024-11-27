#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "PossessionGame/Enums/EUIBar.h"
#include "PlayerHUD.generated.h"

class UBorder;
class UProgressBar;
class UImage;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	bool IsPossessionIndicatorVisible() const { return PossessionIndicator->GetVisibility() == ESlateVisibility::SelfHitTestInvisible; }
	
	void TogglePossessionBar() { ToggleUI(PossessionBar); }
	void ToggleAmmoText() { ToggleUI(AmmoText); }
	void ToggleHpBar() { ToggleUI(HPBar); }
	void UpdateProgressBar(EUIBar Bar, float Current, float Max);
	void UpdateAmmoText(int32 WeaponAmmo);
	void UpdateAmmoText(int32 WeaponAmmo, int32 MaxAmmo);
	void UpdateAmmoText(int32 FirstWeaponAmmo, int32 SecondWeaponAmmo, int32 MaxAmmo);
	void ToggleAbilitiesUI();
	void UpdateAbilityCooldown(int32 WhichAbility, float Secs);
	void UpdateAbilityCooldown(int32 WhichAbility, int32 Kills);
	void ChangeColorOfText(int32 WhichAbility, FLinearColor Color);
	void UpdateFps(int32 Fps);
	void BindCrosshairToggleDelegate();
	void Reset();
	void SetColors(FLinearColor Color);
	void UpdatePossessionIndicator(const bool bState);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PossessionBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Crosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FirstAbilityText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SecondAbilityText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ThirdAbilityText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> FirstAbilityIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SecondAbilityIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ThirdAbilityIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ScreenGlow;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FpsText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PossessionIndicator;

	virtual void NativeConstruct() override;
	void ToggleCrosshair();
	template<typename T>
	void ToggleUI(TObjectPtr<T> UI);
};
