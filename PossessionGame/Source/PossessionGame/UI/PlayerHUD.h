#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	void TogglePossessionBar() { ToggleUI(PossessionBar); }
	void ToggleAmmoText() { ToggleUI(AmmoText); }
	void ToggleHpBar() { ToggleUI(HPBar); }
	void UpdateProgressBar(EUIBar Bar, float Current, float Max);
	void UpdateAmmoText(int32 WeaponAmmo);
	void UpdateAmmoText(int32 WeaponAmmo, int32 MaxAmmo);
	void UpdateAmmoText(int32 FirstWeaponAmmo, int32 SecondWeaponAmmo, int32 MaxAmmo);
	void ToggleAbilitiesUI();
	void ToggleStats();
	void UpdateAbilityCooldown(int32 WhichAbility, float Secs);
	void UpdateAbilityCooldown(int32 WhichAbility, int32 Kills);
	void ChangeColorOfText(int32 WhichAbility, FLinearColor Color);
	void UpdateFps(int32 Fps);
	void BindCrosshairToggleDelegate();
	void Reset();
	void SetColors(FLinearColor Color);
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UProgressBar> PossessionBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UImage> Crosshair;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> AmmoText;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UProgressBar> HPBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> FirstAbilityText;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> SecondAbilityText;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> ThirdAbilityText;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UBorder> FirstAbilityIcon;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UBorder> SecondAbilityIcon;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UBorder> ThirdAbilityIcon;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UImage> ScreenGlow;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> LeftSideDecoration;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> RightSideDecoration;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> FpsText;

	virtual void NativeConstruct() override;
	void ToggleCrosshair();
	template<typename T>
	void ToggleUI(TObjectPtr<T> UI);
};
