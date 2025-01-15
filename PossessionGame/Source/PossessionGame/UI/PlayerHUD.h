#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/EUIBar.h"
#include "PlayerHUD.generated.h"

class UInvalidationBox;
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
	void UpdateProgressBar(const EUIBar InBar, float InCurrent, float InMax) const;
	void UpdateAmmoText(const int32 InWeaponAmmo) const;
	void UpdateAmmoText(const int32 InWeaponAmmo, const int32 InMaxAmmo) const;
	void UpdateAmmoText(const int32 InFirstWeaponAmmo, const int32 InSecondWeaponAmmo, const int32 InMaxAmmo) const;
	void ToggleAbilitiesUI();
	void UpdateAbilityCooldown(const int32 InWhichAbility, const float InSecs, const float InReqSecs) const;
	void UpdateAbilityCooldown(const int32 InWhichAbility, const int32 InKills, const int32 InReqKills) const;
	void UpdateFps(const int32 InFps) const;
	void BindCrosshairToggleDelegate();
	void Reset() const;
	void SetColors(const FLinearColor InColor) const;
	void UpdatePossessionIndicator(const bool bState) const;
	void UpdateRunningIndicator(const bool bState) const;
	void ToggleCriticalDamageIndicator() const;
	void ConfigureUIs() const;
	void PlayDeathFadeOutAnim();
	void CollapseAbilityImage(const int32 Index) const;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PossessionBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> Crosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;
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
	TObjectPtr<UInvalidationBox> FirstAbilityIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> SecondAbilityIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> ThirdAbilityIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ScreenGlow;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FpsText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PossessionIndicator;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> RunningIndicator;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> CriticalDamageIndicator;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DeathFadeOut;

	virtual void NativeConstruct() override;
	void ToggleCrosshair();
	void TriggerAbilityIconReady(const EUIBar InBar, const float InCurrent, const float InMax) const;
	template<typename T>
	void ToggleUI(const TObjectPtr<T>& InUI);
};
