#include "PlayerHUD.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PossessionGame/UI/SecondaryWidgets/AbilitiesPanelUI.h"
#include "PossessionGame/UI/SecondaryWidgets/CrosshairUI.h"
#include "PossessionGame/UI/SecondaryWidgets/InfoPanelUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UPlayerHUD::UpdateProgressBar(const EUIBar InBar, const float InCurrent, const float InMax)
{
	float Progress = InCurrent / InMax;
	switch (InBar)
	{
		case EUIBar::Hp:
			if (!HpBar || !HpWhiteBar || !HpNumberText) return;
			StepHpPercentage = (HpBar->GetPercent() - Progress) * 0.1f;
			HpWhiteBar->SetPercent(HpBar->GetPercent());
			HpBar->SetPercent(Progress);
			HpNumberText->SetText(FText::FromString(FString::Format(TEXT("{0} / {1}"), {static_cast<int32>(InCurrent), static_cast<int32>(InMax)})));
			GetWorld()->GetTimerManager().SetTimer(UpdateWhiteHpBarHandle, this, &UPlayerHUD::UpdateWhiteHpBar, 0.02f, true);
			break;
		case EUIBar::Hijack:
			if (Progress > 1.f) Progress = 1.f;
			if (MID_HijackProgress) MID_HijackProgress->SetScalarParameterValue("Progress", Progress);
			if (HijackChargeText) HijackChargeText->SetText(FText::FromString(FString::FromInt(Progress * 100.f) + "%"));
			break;
		case EUIBar::Xp:
			if (!XpBar || !XpNumberText) return;
			XpNumberText->SetText(FText::FromString(FString::Format(TEXT("{0} / {1}"), {static_cast<int32>(InCurrent), static_cast<int32>(InMax)})));
			XpBar->SetPercent(InCurrent / InMax);
		break;
		case EUIBar::Boss:
			if (!BossHpBar || !BossHpWhiteBar) return;
			BossStepHpPercentage = (BossHpBar->GetPercent() - Progress) * 0.1f;
			BossHpWhiteBar->SetPercent(BossHpBar->GetPercent());
			BossHpBar->SetPercent(Progress);
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
	if (XpBar) XpBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (Crosshair) Crosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (AmmoText) AmmoText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (HpBar) HpBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UpdateHijackIndicator(false);
	GetAbilityPanel()->SetOverlaysVisibility(true);
}

void UPlayerHUD::SetColors(const FLinearColor InColor) const { if (ScreenGlow) ScreenGlow->SetColorAndOpacity(InColor); }

void UPlayerHUD::UpdateHijackIndicator(const bool bState, const FText& InKeyName)
{
	if (HijackChargedImage) HijackChargedImage->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (HijackIndicatorCharged)	HijackIndicatorCharged->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	UpdatePressToHijackKeyBind(FName(), InKeyName);
	if (bState)
	{
		AssetsManager->PlaySound2D(ESound::ReadyToHijack);
		PlayAnimation(HijackIndicatorPopup);
	}
	else if (PressKeyBindSizeBox) PressKeyBindSizeBox->SetRenderOpacity(0.f);
}

void UPlayerHUD::UpdateDashIndicator(const bool bState) const { if (DashIndicator) DashIndicator->SetOpacity(bState ? 1.f : 0.3f); }

void UPlayerHUD::ToggleCriticalDamageIndicator() const
{
	if (!CriticalDamageIndicator) return;
	CriticalDamageIndicator->SetVisibility(CriticalDamageIndicator->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::ConfigureUIs()
{
	if (AnnouncementInv) AnnouncementInv->SetVisibility(ESlateVisibility::Collapsed);
	if (CriticalDamageIndicator) CriticalDamageIndicator->SetVisibility(ESlateVisibility::Collapsed);
	if (InfoPanel) InfoPanel->Setup(CharacterType);
	if (!GetAbilityPanel()) return;
	GetAbilityPanel()->CollapseAbilityIcon(0);
	GetAbilityPanel()->CollapseAbilityIcon(1);
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
	RangerAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
	TricksterAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
	GuardianAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
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

void UPlayerHUD::ShowHijackUnavailable()
{
	if (!HijackUnavailableInv) return;
	PlayAnimation(HijackUnavailableAnim);
	HijackUnavailableInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::ShowOutOfAmmo()
{
	if (!OutOfAmmoInv) return;
	PlayAnimation(OutOfAmmoAnim);
	AssetsManager->PlaySound2D(ESound::OutOfAmmo);
	OutOfAmmoInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::SetAbilityIcon(const int32 InAbilityIndex, const int32 InAbilityIconIndex) { GetAbilityPanel()->SetAbilityIcon(InAbilityIndex, InAbilityIconIndex); }

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
	DelegateManager->OnKeyBindChange.AddUObject(this, &UPlayerHUD::UpdatePressToHijackKeyBind);
	DelegateManager->OnOutOfAmmo.BindUObject(this, &UPlayerHUD::ShowOutOfAmmo);
	DelegateManager->RequirePlayerAnnouncementInfo.BindUObject(this, &UPlayerHUD::PlayAnnouncement);
	if (HijackProgressBar) MID_HijackProgress = HijackProgressBar->GetDynamicMaterial();
	UpdateHijackIndicator(false);
	SetLevelText(Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetPlayerLevel());
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

void UPlayerHUD::UpdatePressToHijackKeyBind(const FName& InKeyBindName, const FText& InKeyBindText) const
{
	if (PressKeyBindText && InKeyBindName.IsEqual("Hijack")) PressKeyBindText->SetText(FText::Format(FText::FromString("{0}"), InKeyBindText));
}

void UPlayerHUD::PlayAnnouncement(const FText& InText)
{
	if (!AnnouncementInv || !AnnouncementText) return;
	AnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AnnouncementText->SetText(InText);
	PlayAnimation(HijackedPopup);
	if (InText.ToString().Equals("Hijacked")) PlayAnimation(HijackIndicatorPopup, 0, 1, EUMGSequencePlayMode::Reverse);
}

template<typename T>
void UPlayerHUD::ToggleUI(const TObjectPtr<T>& InUI)
{
	if (!InUI) return;
	InUI->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? InUI->SetVisibility(ESlateVisibility::Collapsed) : InUI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
