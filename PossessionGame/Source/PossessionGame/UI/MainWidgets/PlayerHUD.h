#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EUIBar.h"
#include "PlayerHUD.generated.h"

class UPrimaryDataAssetsManager;
class UInfoPanelUI;
class UCrosshairUI;
class UAbility;
class UAbilitiesPanelUI;
enum class ECharacterType : uint8;
class UOverlay;
class AEnemy;
class UInvalidationBox;
class UProgressBar;
class UImage;
class UTextBlock;

UCLASS()
class POSSESSIONGAME_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetPlayerRef(const TObjectPtr<AEnemy>& InPlayerRef) { Player = InPlayerRef; }
	void UpdateProgressBar(const EUIBar InBar, const float InCurrent, const float InMax);
	void UpdateAmmoText(const int32 InWeaponAmmo) const;
	void UpdateAmmoText(const int32 InWeaponAmmo, const int32 InMaxAmmo) const;
	void UpdateAmmoText(const int32 InFirstWeaponAmmo, const int32 InSecondWeaponAmmo, const int32 InMaxAmmo) const;
	void UpdateAbilityUI(const TObjectPtr<UAbility>& InAbility);
	void UpdateFps(const int32 InFps) const;
	void Reset();
	void SetColors(const FLinearColor InColor) const;
	void UpdateHijackIndicator(const bool bState, const FText& InKeyName = FText());
	void UpdateDashIndicator(const bool bState) const;
	void ToggleCriticalDamageIndicator() const;
	void ConfigureUIs();
	void PlayFadeOutAnim(const bool bState);
	void UpdateReceiveDamageInv();
	void SetCharacterType(const ECharacterType InCharacterType);
	void SetCurrentCrosshair() const;
	void ToggleInfoPanel(const bool bState) const;
	void ShowPossessionUnavailable();
	void ShowOutOfAmmo();
private:
	bool bIsHidden = false;
	float StepHpPercentage;
	float BossStepHpPercentage;
	ECharacterType CharacterType;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> DamageTickTexture;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UTexture2D> CriticalDamageTickTexture;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UAbilitiesPanelUI> RangerAbilitiesPanel;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UAbilitiesPanelUI> TricksterAbilitiesPanel;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UAbilitiesPanelUI> GuardianAbilitiesPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PossessionBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> XpBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> Crosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> RangerCrosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> TricksterCrosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> GuardianCrosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> DamageTickInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> AnnouncementInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AnnouncementText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DamageTickImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> ReceiveDamageInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> LevelUpAnnouncementInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HpBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HpWhiteBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> BossHpWhiteBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> BossHpBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ScreenGlow;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FpsText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DashIndicator;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> CriticalDamageIndicator;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> BossHpBarInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PressKeyBindText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HpNumberText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> XpNumberText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelNumberText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> InfoPanelInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> OutOfAmmoInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> PossessionUnavailableInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HijackProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HijackChargedImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HijackIndicatorUncharged;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HijackIndicatorCharged;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HijackChargeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInfoPanelUI> InfoPanel;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ReceiveDamageFadeOut;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DamageTickFadeOut;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PossessionIndicatorPopup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PressToPossessPopup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PossessedPopup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LevelUpAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PossessionUnavailableAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> OutOfAmmoAnim;
	UPROPERTY()
	TObjectPtr<AEnemy> Player;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_HijackProgress;
	UPROPERTY()
	FTimerHandle UpdateWhiteHpBarHandle;
	UPROPERTY()
	FTimerHandle UpdateBossWhiteHpBarHandle;

	TObjectPtr<UAbilitiesPanelUI> GetAbilityPanel();
	
	virtual void NativeConstruct() override;
	void ShowDamageTick(const bool bIsCritical);
	void LevelUp(const int32 InLevel);
	void PlayLevelUpAnim();
	void SetLevelText(const int32 InLevel) const;
	void UpdateBossHpBar(const float InCurrent, const float InMax);
	void EnableBossHpBar() const;
	void DisableBossHpBar() const;
	void UpdateWhiteHpBar();
	void UpdateBossWhiteHpBar();
	void UpdatePressToHijackKeyBind(const FName& InKeyBindName, const FText& InKeyBindText) const;
	void PlayAnnouncement(const FText& InText);
	template<typename T>
	void ToggleUI(const TObjectPtr<T>& InUI);
};
