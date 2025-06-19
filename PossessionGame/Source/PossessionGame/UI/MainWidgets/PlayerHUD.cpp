#include "PlayerHUD.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PossessionGame/UI/SecondaryWidgets/AbilitiesPanelUI.h"
#include "PossessionGame/UI/SecondaryWidgets/CrosshairUI.h"
#include "PossessionGame/UI/SecondaryWidgets/InfoPanelUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

bool UPlayerHUD::GetPossessionIndicatorIsShowing() const { return PossessionIndicator ? PossessionIndicator->GetVisibility() == ESlateVisibility::SelfHitTestInvisible : false; }

void UPlayerHUD::UpdateProgressBar(const EUIBar InBar, const float InCurrent, const float InMax)
{
	switch (InBar)
	{
		case EUIBar::Hp:
			if (!HpBar || !HpWhiteBar || !HpNumberText) return;
			StepHpPercentage = (HpBar->GetPercent() - InCurrent / InMax) * 0.1f;
			HpWhiteBar->SetPercent(HpBar->GetPercent());
			HpBar->SetPercent(InCurrent / InMax);
			HpNumberText->SetText(FText::FromString(FString::Format(TEXT("{0} / {1}"), {static_cast<int32>(InCurrent), static_cast<int32>(InMax)})));
			GetWorld()->GetTimerManager().SetTimer(UpdateWhiteHpBarHandle, this, &UPlayerHUD::UpdateWhiteHpBar, 0.02f, true);
			break;
		case EUIBar::Possession: if (PossessionBar) PossessionBar->SetPercent(InCurrent / InMax); break;
		case EUIBar::Xp:
			if (!XpBar || !XpNumberText) return;
			XpNumberText->SetText(FText::FromString(FString::Format(TEXT("{0} / {1}"), {static_cast<int32>(InCurrent), static_cast<int32>(InMax)})));
			XpBar->SetPercent(InCurrent / InMax);
		break;
		case EUIBar::Boss:
			if (!BossHpBar || !BossHpWhiteBar) return;
			BossStepHpPercentage = (BossHpBar->GetPercent() - InCurrent / InMax) * 0.1f;
			BossHpWhiteBar->SetPercent(BossHpBar->GetPercent());
			BossHpBar->SetPercent(InCurrent / InMax);
			GetWorld()->GetTimerManager().SetTimer(UpdateBossWhiteHpBarHandle, this, &UPlayerHUD::UpdateBossWhiteHpBar, 0.02f, true);
		default: ;
	}
}

void UPlayerHUD::UpdateAmmoText(const int32 InWeaponAmmo) const
{
	if (!AmmoText) return;
	if (InWeaponAmmo == -1) AmmoText->SetText(FText::FromString(TEXT("∞")));
	else AmmoText->SetText(FText::FromString(FString::Format(TEXT("{0}"), {InWeaponAmmo})));
}

void UPlayerHUD::UpdateAmmoText(const int32 InWeaponAmmo, const int32 InMaxAmmo) const
{
	if (!AmmoText) return;
	AmmoText->SetText(FText::FromString(FString::Format(TEXT("{0} / {1}"), {InWeaponAmmo, InMaxAmmo})));
}

void UPlayerHUD::UpdateAmmoText(const int32 InFirstWeaponAmmo, const int32 InSecondWeaponAmmo, const int32 InMaxAmmo) const
{
	if (!AmmoText) return;
	AmmoText->SetText(FText::FromString(FString::Format(TEXT("{0} / {1} / {2}"), {InFirstWeaponAmmo, InSecondWeaponAmmo, InMaxAmmo})));
}

void UPlayerHUD::UpdateAbilityUI(const TObjectPtr<UAbility>& InAbility) { if (GetAbilityPanel()) GetAbilityPanel()->UpdateAbilityUI(InAbility); }

void UPlayerHUD::UpdateFps(const int32 InFps) const
{
	if (!FpsText) return;
	FpsText->SetText(FText::FromString(FString::Format(TEXT("FPS: {0}"), {InFps})));
}

void UPlayerHUD::Reset()
{
	if (PossessionBar) PossessionBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (XpBar) XpBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (Crosshair) Crosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (AmmoText) AmmoText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (HpBar) HpBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UpdatePossessionIndicator(false);
	GetAbilityPanel()->SetOverlaysVisibility(true);
}

void UPlayerHUD::SetColors(const FLinearColor InColor) const { if (ScreenGlow) ScreenGlow->SetColorAndOpacity(InColor); }

void UPlayerHUD::UpdatePossessionIndicator(const bool bState, const FText& InKeyName)
{
	if (PossessionIndicator) PossessionIndicator->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (PressKeyBindInv) PressKeyBindInv->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	UpdatePressToPossessKeyBind(FName(), InKeyName);
	if (PossessionIndicatorOverlay)	PossessionIndicatorOverlay->SetVisibility(bState ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	if (bState)
	{
		AssetsManager->PlaySound2D(ESound::ReadyToPossess);
		PlayAnimation(PossessionIndicatorPopup);
		PlayAnimation(PressToPossessPopup);
	}
}

void UPlayerHUD::UpdateDashIndicator(const bool bState) const { if (DashIndicator) DashIndicator->SetOpacity(bState ? 1.f : 0.3f); }

void UPlayerHUD::ToggleCriticalDamageIndicator() const
{
	if (!CriticalDamageIndicator) return;
	CriticalDamageIndicator->SetVisibility(CriticalDamageIndicator->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::ConfigureUIs()
{
	if (PressKeyBindInv) PressKeyBindInv->SetVisibility(ESlateVisibility::Collapsed);
	if (AnnouncementInv) AnnouncementInv->SetVisibility(ESlateVisibility::Collapsed);
	if (PossessionIndicator) PossessionIndicator->SetVisibility(ESlateVisibility::Collapsed);
	if (CriticalDamageIndicator) CriticalDamageIndicator->SetVisibility(ESlateVisibility::Collapsed);
	if (InfoPanel) InfoPanel->Setup(CharacterType);
	if (!GetAbilityPanel()) return;
	GetAbilityPanel()->CollapseAbilityIcon(0);
	GetAbilityPanel()->CollapseAbilityIcon(1);
	GetAbilityPanel()->CollapseAbilityIcon(2);
}

void UPlayerHUD::PlayFadeOutAnim(const bool bState)
{
	if (bState == bIsHidden) return;
	bIsHidden = bState;
	PlayAnimation(FadeOutAnim, GetAnimationCurrentTime(FadeOutAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UPlayerHUD::UpdateReceiveDamageInv()
{
	if (ReceiveDamageInv) ReceiveDamageInv->SetRenderOpacity(1.f);
	PlayAnimation(ReceiveDamageFadeOut);
}

void UPlayerHUD::SetCharacterType(const ECharacterType InCharacterType)
{
	CharacterType = InCharacterType;
	switch (InCharacterType)
	{
		case ECharacterType::Ranger:
			if (RangerAbilitiesPanel) RangerAbilitiesPanel->SetAsCurrent();
			if (TricksterAbilitiesPanel) TricksterAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
			if (GuardianAbilitiesPanel) GuardianAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
			break;
		case ECharacterType::Trickster:
			if (TricksterAbilitiesPanel) TricksterAbilitiesPanel->SetAsCurrent();
			if (RangerAbilitiesPanel) RangerAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
			if (GuardianAbilitiesPanel) GuardianAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
			break;
		default:
			if (GuardianAbilitiesPanel) GuardianAbilitiesPanel->SetAsCurrent();
			if (RangerAbilitiesPanel) RangerAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
			if (TricksterAbilitiesPanel) TricksterAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	SetCurrentCrosshair();
}

void UPlayerHUD::SetCurrentCrosshair() const
{
	switch(CharacterType)
	{
		case ECharacterType::Ranger:
			if (RangerCrosshair) RangerCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (TricksterCrosshair) TricksterCrosshair->SetVisibility(ESlateVisibility::Collapsed);
			if (GuardianCrosshair) GuardianCrosshair->SetVisibility(ESlateVisibility::Collapsed);
			break;
		case ECharacterType::Trickster:
			if (RangerCrosshair) RangerCrosshair->SetVisibility(ESlateVisibility::Collapsed);
			if (TricksterCrosshair) TricksterCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (GuardianCrosshair) GuardianCrosshair->SetVisibility(ESlateVisibility::Collapsed);
			break;
		default:
			if (RangerCrosshair) RangerCrosshair->SetVisibility(ESlateVisibility::Collapsed);
			if (TricksterCrosshair) TricksterCrosshair->SetVisibility(ESlateVisibility::Collapsed);
			if (GuardianCrosshair) GuardianCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UPlayerHUD::ToggleInfoPanel(const bool bState) const
{
	if (!InfoPanelInv) return;
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	InfoPanelInv->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UPlayerHUD::ShowPossessionUnavailable()
{
	if (!PossessionUnavailableInv) return;
	PlayAnimation(PossessionUnavailableAnim);
	PossessionUnavailableInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::ShowOutOfAmmo()
{
	if (!OutOfAmmoInv) return;
	PlayAnimation(OutOfAmmoAnim);
	AssetsManager->PlaySound2D(ESound::OutOfAmmo);
	OutOfAmmoInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

TObjectPtr<UAbilitiesPanelUI> UPlayerHUD::GetAbilityPanel()
{
	switch (CharacterType)
	{
		case ECharacterType::Ranger: return RangerAbilitiesPanel;
		case ECharacterType::Guardian: return GuardianAbilitiesPanel;
		default: return TricksterAbilitiesPanel;
	}
}

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnPlayerDamageDealt.BindUObject(this, &UPlayerHUD::ShowDamageTick);
	DelegateManager->OnLevelUp.AddUObject(this, &UPlayerHUD::LevelUp);
	DelegateManager->OnBossDamaged.BindUObject(this, &UPlayerHUD::UpdateBossHpBar);
	DelegateManager->OnBossSpawn.BindUObject(this, &UPlayerHUD::EnableBossHpBar);
	DelegateManager->OnBossDeath.AddUObject(this, &UPlayerHUD::DisableBossHpBar);
	DelegateManager->OnKeyBindChange.AddUObject(this, &UPlayerHUD::UpdatePressToPossessKeyBind);
	DelegateManager->OnOutOfAmmo.BindUObject(this, &UPlayerHUD::ShowOutOfAmmo);
	DelegateManager->RequirePlayerAnnouncementInfo.BindUObject(this, &UPlayerHUD::PlayAnnouncement);
}

void UPlayerHUD::ShowDamageTick(const bool bIsCritical)
{
	if (DamageTickInv) DamageTickInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (DamageTickImage && CriticalDamageTickTexture && DamageTickTexture) DamageTickImage->SetBrushFromTexture(bIsCritical ? CriticalDamageTickTexture : DamageTickTexture);
	PlayAnimation(DamageTickFadeOut);
}

void UPlayerHUD::LevelUp(const int32 InLevel)
{
	PlayLevelUpAnim();
	SetLevelText(InLevel);
}

void UPlayerHUD::PlayLevelUpAnim()
{
	if (!LevelUpAnnouncementInv) return;
	LevelUpAnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimation(LevelUpAnim);
}

void UPlayerHUD::SetLevelText(const int32 InLevel) const { if (LevelNumberText) LevelNumberText->SetText(FText::FromString(FString::FromInt(InLevel))); }

void UPlayerHUD::UpdateBossHpBar(const float InCurrent, const float InMax)
{
	if (!BossHpBarInv) return;
	UpdateProgressBar(EUIBar::Boss, InCurrent, InMax);
}

void UPlayerHUD::EnableBossHpBar() const { if (BossHpBarInv) BossHpBarInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible); }

void UPlayerHUD::DisableBossHpBar() const { if (BossHpBarInv) BossHpBarInv->SetVisibility(ESlateVisibility::Collapsed); }

void UPlayerHUD::UpdateWhiteHpBar()
{
	if (!HpWhiteBar || !HpBar) return;
	float NewPercentage = HpWhiteBar->GetPercent() - StepHpPercentage;
	if (NewPercentage <= HpBar->GetPercent())
	{
		NewPercentage = HpBar->GetPercent();
		GetWorld()->GetTimerManager().ClearTimer(UpdateWhiteHpBarHandle);
	}
	HpWhiteBar->SetPercent(NewPercentage);
}

void UPlayerHUD::UpdateBossWhiteHpBar()
{
	if (!BossHpWhiteBar || !BossHpBar) return;
	float NewPercentage = BossHpWhiteBar->GetPercent() - BossStepHpPercentage;
	if (NewPercentage <= BossHpBar->GetPercent())
	{
		NewPercentage = BossHpBar->GetPercent();
		GetWorld()->GetTimerManager().ClearTimer(UpdateBossWhiteHpBarHandle);
	}
	BossHpWhiteBar->SetPercent(NewPercentage);
}

void UPlayerHUD::UpdatePressToPossessKeyBind(const FName& InKeyBindName, const FText& InKeyBindText) const
{
	if (PressKeyBindText && InKeyBindName.IsEqual("Hijack")) PressKeyBindText->SetText(FText::Format(FText::FromString("Press {0} to possess"), InKeyBindText));
}

void UPlayerHUD::PlayAnnouncement(const FText& InText)
{
	if (!AnnouncementInv || !AnnouncementText) return;
	AnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AnnouncementText->SetText(InText);
	PlayAnimation(PossessedPopup);
}

template<typename T>
void UPlayerHUD::ToggleUI(const TObjectPtr<T>& InUI)
{
	if (!InUI) return;
	InUI->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? InUI->SetVisibility(ESlateVisibility::Collapsed) : InUI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
