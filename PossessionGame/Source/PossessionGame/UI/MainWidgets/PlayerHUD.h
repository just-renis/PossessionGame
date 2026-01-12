#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EUIBar.h"
#include "PlayerHUD.generated.h"

enum class ESound : uint16;
enum class EEnemyHpBar : uint8;
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
	void Reset();
	void UpdateHijackIndicator(const bool bState, const FText& InKeyName = FText());
	void ToggleCriticalDamageIndicator() const;
	void ConfigureUIs();
	void PlayFadeOutAnim(const bool bState);
	void UpdateReceiveDamageInv();
	void SetCharacterType(const ECharacterType InCharacterType);
	void SetCurrentCrosshair() const;
	void ToggleInfoPanel(const bool bState) const;
	void ShowMinorAnnouncement(const FText& InText, const ESound InSound);
	void ShowOutOfAmmo();
	void SetAbilityIcon(const int32 InAbilityIndex, const int32 InAbilityIconIndex);
	void PlayHijackAnim(const bool bState, const float InPlaybackSpeed);
	void CancelHijackInitiateAnim() { PlayAnimation(HijackInitiateAnim, 1, 1, EUMGSequencePlayMode::Reverse); }
private:
	bool bIsHidden = false;
	bool bDisplayOutOfAmmoAvailable = true;
	bool bDisplayMinorAnnouncementAvailable = true;
	float StepHpPercentage;
	float GlobalStepHpPercentage;
	float StepXpPercentage;
	float StepUpgradePercentage;
	float MinorAnnouncementDisplayCooldown = 0.3f;
	float OutOfAmmoDisplayCooldown = 0.5f;
	ECharacterType CharacterType;
	FTimerHandle UpdateHpWhiteBarHandle;
	FTimerHandle UpdateGlobalHpWhiteBarHandle;
	FTimerHandle UpdateXpBarHandle;
	FTimerHandle UpdateUpgradeBarHandle;
	FTimerHandle OutOfAmmoDisplayHandle;
	FTimerHandle MinorAnnouncementDisplayHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Hp Bars Data", meta = (AllowPrivateAccess = true))
	TMap<EEnemyHpBar, TObjectPtr<UTexture2D>> EnemyHpBars;
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
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UAbilitiesPanelUI> BreacherAbilitiesPanel;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UAbilitiesPanelUI> WraithAbilitiesPanel;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UAbilitiesPanelUI> TankAbilitiesPanel;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UAbilitiesPanelUI> SpyAbilitiesPanel;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UAbilitiesPanelUI> SamuraiAbilitiesPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageNumberText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> XpBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> XpWhiteBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> Crosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> RangerCrosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> TricksterCrosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> GuardianCrosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> BreacherCrosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> WraithCrosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> TankCrosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> SpyCrosshair;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairUI> SamuraiCrosshair;
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
	TObjectPtr<UProgressBar> UpgradeBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> UpgradeWhiteBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> GlobalHpWhiteBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> GlobalHpBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> CriticalDamageIndicator;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> GlobalHpBarInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PressKeyBindText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HpNumberText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> XpNumberText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelNumberText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeKeyBindText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> InfoPanelInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> OutOfAmmoInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInvalidationBox> MinorAnnouncementInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MinorAnnouncementText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> UpgradeTextInv;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UpgradeAmountText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HijackProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HijackChargedImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HijackIndicatorCharged;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HijackChargeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> PressKeyBindSizeBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInfoPanelUI> InfoPanel;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ReceiveDamageFadeOut;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DamageTickFadeOut;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HijackIndicatorPopup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HijackedPopup;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LevelUpAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> MinorAnnouncementAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> OutOfAmmoAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DamageReceivedAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HijackInitiateAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> UpgradePopupAnim;
	UPROPERTY()
	TObjectPtr<AEnemy> Player;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_HijackProgress;

	TObjectPtr<UAbilitiesPanelUI> GetAbilityPanel();
	
	void EnableOutOfAmmoDisplayAvailability() { bDisplayOutOfAmmoAvailable = true; }
	void EnableMinorAnnouncementDisplayAvailability() { bDisplayMinorAnnouncementAvailable = true; }
	virtual void NativeConstruct() override;
	void ShowDamageTick(const bool bIsCritical);
	void LevelUp();
	void PlayLevelUpAnim();
	void SetLevelText() const;
	void UpdateGlobalHpBar(const float InCurrent, const float InMax);
	void EnableGlobalHpBar(const EEnemyHpBar InHpBarType) const;
	void DisableGlobalHpBar() const;
	void UpdateWhiteHpBar();
	void UpdateGlobalWhiteHpBar();
	void UpdateXpBar();
	void UpdateUpgradeBar();
	void UpdatePressToHijackKeyBind(const FName& InKeyBindName, const FText& InKeyBindText) const;
	void PlayAnnouncement(const FText& InText);
	void SetDamageNumber(float InDamage, const FLinearColor InColor);
	void CollapseAllAbilitiesPanels() const;
	void CollapseAllCrosshairs() const;
	virtual void NativeDestruct() override;
	void UpdateUpgradeAmountText(const int32 InAmount);
	void UpdateUpgradeKeyBindText(const FName& InKeyBindName, const FText& InKeyName) const;
	template<typename T>
	void ToggleUI(const TObjectPtr<T>& InUI);
};
