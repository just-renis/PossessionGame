#include "PlayerHUD.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/UI/SecondaryWidgets/AbilitiesPanelUI.h"
#include "PossessionGame/UI/SecondaryWidgets/CrosshairUI.h"
#include "PossessionGame/UI/SecondaryWidgets/InfoPanelUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

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
			GetWorld()->GetTimerManager().SetTimer(UpdateHpWhiteBarHandle, this, &UPlayerHUD::UpdateWhiteHpBar, 0.02f, true);
			break;
		case EUIBar::Hijack:
			if (Progress > 1.f) Progress = 1.f;
			if (MID_HijackProgress) MID_HijackProgress->SetScalarParameterValue("Progress", Progress);
			if (HijackChargeText) HijackChargeText->SetText(FText::FromString(FString::FromInt(Progress * 100.f) + "%"));
			break;
		case EUIBar::Xp:
			if (!XpBar || !XpWhiteBar || !XpNumberText) return;
			StepXpPercentage = (XpWhiteBar->GetPercent() - Progress) * 0.1f;
			XpBar->SetPercent(XpWhiteBar->GetPercent());
			XpWhiteBar->SetPercent(Progress);
			GetWorld()->GetTimerManager().SetTimer(UpdateXpBarHandle, this, &UPlayerHUD::UpdateXpBar, 0.02f, true);
			XpNumberText->SetText(FText::FromString(FString::Format(TEXT("{0} / {1}"), {static_cast<int32>(InCurrent), static_cast<int32>(InMax)})));
			break;
		case EUIBar::Boss:
			if (!GlobalHpBar || !GlobalHpWhiteBar) return;
			GlobalStepHpPercentage = (GlobalHpBar->GetPercent() - Progress) * 0.1f;
			GlobalHpWhiteBar->SetPercent(GlobalHpBar->GetPercent());
			GlobalHpBar->SetPercent(Progress);
			GetWorld()->GetTimerManager().SetTimer(UpdateGlobalHpWhiteBarHandle, this, &UPlayerHUD::UpdateGlobalWhiteHpBar, 0.02f, true);
			break;
		case EUIBar::Upgrade:
			if (!UpgradeBar || !UpgradeWhiteBar) return;
			StepUpgradePercentage = (UpgradeWhiteBar->GetPercent() - Progress) * 0.1f;
			UpgradeBar->SetPercent(UpgradeWhiteBar->GetPercent());
			UpgradeWhiteBar->SetPercent(Progress);
			GetWorld()->GetTimerManager().SetTimer(UpdateUpgradeBarHandle, this, &UPlayerHUD::UpdateUpgradeBar, 0.02f, true);
			break;
		default: ;
	}
}

void UPlayerHUD::UpdateAmmoText(const int32 InWeaponAmmo) const
{
	if (!AmmoText) return;
	AmmoText->SetText(FText::FromString(FString::Format(TEXT("{0}"), {InWeaponAmmo < 0 ? 0 : InWeaponAmmo})));
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

void UPlayerHUD::Reset()
{
	if (XpBar) XpBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (Crosshair) Crosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (AmmoText) AmmoText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (HpBar) HpBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UpdateHijackIndicator(false);
	GetAbilityPanel()->SetOverlaysVisibility(true);
}

void UPlayerHUD::SetColors(const ECharacterType InCharacterName) const
{
	if (!ScreenGlow) return;
	if (const FLinearColor* Color = CharacterScreenGlowColors.Find(InCharacterName)) ScreenGlow->SetColorAndOpacity(*Color);
	else ScreenGlow->SetColorAndOpacity(FLinearColor::White);
}

void UPlayerHUD::UpdateHijackIndicator(const bool bState, const FText& InKeyName)
{
	if (HijackChargedImage) HijackChargedImage->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (HijackIndicatorCharged)	HijackIndicatorCharged->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	UpdatePressToHijackKeyBind(FName("Hijack"), InKeyName);
	if (bState)
	{
		AssetsManager->PlaySound2D(ESound::ReadyToHijack);
		PlayAnimation(HijackIndicatorPopup);
	}
	else if (PressKeyBindSizeBox) PressKeyBindSizeBox->SetRenderOpacity(0.f);
}

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
	CollapseAllAbilitiesPanels();
	switch (InCharacterType)
	{
		case ECharacterType::Ranger: RangerAbilitiesPanel->SetAsCurrent(); break;
		case ECharacterType::Trickster: TricksterAbilitiesPanel->SetAsCurrent(); break;
		case ECharacterType::Breacher: BreacherAbilitiesPanel->SetAsCurrent(); break;
		case ECharacterType::Wraith: WraithAbilitiesPanel->SetAsCurrent(); break;
		case ECharacterType::Tank: TankAbilitiesPanel->SetAsCurrent(); break;
		case ECharacterType::Spy: SpyAbilitiesPanel->SetAsCurrent(); break;
		case ECharacterType::Samurai: SamuraiAbilitiesPanel->SetAsCurrent(); break;
		default: GuardianAbilitiesPanel->SetAsCurrent();
	}
	SetCurrentCrosshair();
}

void UPlayerHUD::SetCurrentCrosshair() const
{
	CollapseAllCrosshairs();
	switch(CharacterType)
	{
		case ECharacterType::Ranger: RangerCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible); break;
		case ECharacterType::Trickster: TricksterCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible); break;
		case ECharacterType::Breacher: BreacherCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible); break;
		case ECharacterType::Wraith: WraithCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible); break;
		case ECharacterType::Tank: TankCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible); break;
		case ECharacterType::Spy: SpyCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible); break;
		case ECharacterType::Samurai: SamuraiCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible); break;
		default: GuardianCrosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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

void UPlayerHUD::ShowMinorAnnouncement(const FText& InText, const ESound InSound)
{
	if (!MinorAnnouncementInv || !MinorAnnouncementText || !bDisplayMinorAnnouncementAvailable) return;
	AssetsManager->PlaySound2D(InSound);
	bDisplayMinorAnnouncementAvailable = false;
	MinorAnnouncementText->SetText(InText);
	PlayAnimation(MinorAnnouncementAnim);
	MinorAnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer(MinorAnnouncementDisplayHandle, this, &UPlayerHUD::EnableMinorAnnouncementDisplayAvailability, MinorAnnouncementDisplayCooldown);
}

void UPlayerHUD::ShowOutOfAmmo()
{
	if (!OutOfAmmoInv || !bDisplayOutOfAmmoAvailable) return;
	bDisplayOutOfAmmoAvailable = false;
	PlayAnimation(OutOfAmmoAnim);
	AssetsManager->PlaySound2D(ESound::NotSelectable);
	OutOfAmmoInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer(OutOfAmmoDisplayHandle, this, &UPlayerHUD::EnableOutOfAmmoDisplayAvailability, OutOfAmmoDisplayCooldown);
}

void UPlayerHUD::SetAbilityIcon(const int32 InAbilityIndex, const int32 InAbilityIconIndex) { GetAbilityPanel()->SetAbilityIcon(InAbilityIndex, InAbilityIconIndex); }

TObjectPtr<UAbilitiesPanelUI> UPlayerHUD::GetAbilityPanel()
{
	switch (CharacterType)
	{
		case ECharacterType::Ranger: return RangerAbilitiesPanel;
		case ECharacterType::Trickster: return TricksterAbilitiesPanel;
		case ECharacterType::Breacher: return BreacherAbilitiesPanel;
		case ECharacterType::Wraith: return WraithAbilitiesPanel;
		case ECharacterType::Tank: return TankAbilitiesPanel;
		case ECharacterType::Spy: return SpyAbilitiesPanel;
		case ECharacterType::Samurai: return SamuraiAbilitiesPanel;
		default: return GuardianAbilitiesPanel;
	}
}

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	AssetsManager = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>();
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnPlayerDamageDealt.BindUObject(this, &UPlayerHUD::ShowDamageTick);
	DelegateManager->OnLevelUp.AddUObject(this, &UPlayerHUD::LevelUp);
	DelegateManager->UpdateGlobalHpBar.BindUObject(this, &UPlayerHUD::UpdateGlobalHpBar);
	DelegateManager->OnGlobalHpBarSpawn.BindUObject(this, &UPlayerHUD::EnableGlobalHpBar);
	DelegateManager->OnBossDeath.AddUObject(this, &UPlayerHUD::DisableGlobalHpBar);
	DelegateManager->OnKeyBindChange.AddUObject(this, &UPlayerHUD::UpdatePressToHijackKeyBind);
	DelegateManager->OnKeyBindChange.AddUObject(this, &UPlayerHUD::UpdateUpgradeKeyBindText);
	DelegateManager->RequirePlayerAnnouncementInfo.BindUObject(this, &UPlayerHUD::PlayAnnouncement);
	DelegateManager->RequestMinorAnnouncement.BindUObject(this, &UPlayerHUD::ShowMinorAnnouncement);
	DelegateManager->OnPlayerDamageTaken.BindUObject(this, &UPlayerHUD::SetDamageNumber);
	DelegateManager->OnUpgradeAmountChanged.BindUObject(this, &UPlayerHUD::UpdateUpgradeAmountText);
	if (HijackProgressBar) MID_HijackProgress = HijackProgressBar->GetDynamicMaterial();
	UpdateHijackIndicator(false);
	UpdateUpgradeKeyBindText(FName("OpenUpgrade"), GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyBindText(FName("OpenUpgrade")));
	SetLevelText();
}

void UPlayerHUD::ShowDamageTick(const bool bIsCritical)
{
	AssetsManager->PlaySound2D(bIsCritical ? ESound::CriticalDamageTick : ESound::DamageTick);
	if (DamageTickInv) DamageTickInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (DamageTickImage && CriticalDamageTickTexture && DamageTickTexture) DamageTickImage->SetBrushFromTexture(bIsCritical ? CriticalDamageTickTexture : DamageTickTexture);
	PlayAnimation(DamageTickFadeOut);
}

void UPlayerHUD::LevelUp()
{
	PlayLevelUpAnim();
	SetLevelText();
}

void UPlayerHUD::PlayLevelUpAnim()
{
	if (!LevelUpAnnouncementInv) return;
	LevelUpAnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimation(LevelUpAnim);
}

void UPlayerHUD::SetLevelText() const { if (LevelNumberText) LevelNumberText->SetText(FText::FromString(FString::FromInt(GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevel()))); }

void UPlayerHUD::UpdateGlobalHpBar(const float InCurrent, const float InMax)
{
	if (!GlobalHpBarInv) return;
	UpdateProgressBar(EUIBar::Boss, InCurrent, InMax);
}

void UPlayerHUD::EnableGlobalHpBar(const EEnemyHpBar InHpBarType) const
{
	if (!GlobalHpBarInv || !GlobalHpBar) return;
	FProgressBarStyle CurrentStyle = GlobalHpBar->GetWidgetStyle();
	CurrentStyle.FillImage.SetResourceObject(EnemyHpBars[InHpBarType]);
	GlobalHpBar->SetWidgetStyle(CurrentStyle);
	GlobalHpBar->SetPercent(1.f);
	GlobalHpWhiteBar->SetPercent(1.f);
	GlobalHpBarInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::DisableGlobalHpBar() const { if (GlobalHpBarInv) GlobalHpBarInv->SetVisibility(ESlateVisibility::Collapsed); }

void UPlayerHUD::UpdateWhiteHpBar()
{
	if (!HpWhiteBar || !HpBar) return;
	float NewPercentage = HpWhiteBar->GetPercent() - StepHpPercentage;
	if (NewPercentage <= HpBar->GetPercent())
	{
		NewPercentage = HpBar->GetPercent();
		GetWorld()->GetTimerManager().ClearTimer(UpdateHpWhiteBarHandle);
	}
	HpWhiteBar->SetPercent(NewPercentage);
}

void UPlayerHUD::UpdateGlobalWhiteHpBar()
{
	if (!GlobalHpWhiteBar || !GlobalHpBar) return;
	float NewPercentage = GlobalHpWhiteBar->GetPercent() - GlobalStepHpPercentage;
	if (NewPercentage <= GlobalHpBar->GetPercent())
	{
		NewPercentage = GlobalHpBar->GetPercent();
		GetWorld()->GetTimerManager().ClearTimer(UpdateGlobalHpWhiteBarHandle);
	}
	GlobalHpWhiteBar->SetPercent(NewPercentage);
}

void UPlayerHUD::UpdateXpBar()
{
	if (!XpWhiteBar || !XpBar) return;
	float NewPercentage = XpBar->GetPercent() - StepXpPercentage;
	if (NewPercentage <= XpWhiteBar->GetPercent())
	{
		NewPercentage = XpWhiteBar->GetPercent();
		GetWorld()->GetTimerManager().ClearTimer(UpdateXpBarHandle);
	}
	XpBar->SetPercent(NewPercentage);
}

void UPlayerHUD::UpdateUpgradeBar()
{
	if (!UpgradeWhiteBar || !UpgradeBar) return;
	float NewPercentage = UpgradeBar->GetPercent() - StepUpgradePercentage;
	if (NewPercentage <= UpgradeWhiteBar->GetPercent())
	{
		NewPercentage = UpgradeWhiteBar->GetPercent();
		GetWorld()->GetTimerManager().ClearTimer(UpdateUpgradeBarHandle);
	}
	UpgradeBar->SetPercent(NewPercentage);
}

void UPlayerHUD::UpdatePressToHijackKeyBind(const FName& InKeyBindName, const FText& InKeyBindText) const
{
	if (!InKeyBindName.IsEqual("Hijack") || !PressKeyBindText) return;
	PressKeyBindText->SetText(FText::Format(FText::FromString("{0}"), InKeyBindText));
}

void UPlayerHUD::PlayAnnouncement(const FText& InText)
{
	if (!AnnouncementInv || !AnnouncementText) return;
	AnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AnnouncementText->SetText(InText);
	PlayAnimation(HijackedPopup);
	if (InText.ToString().Equals("Hijacked")) PlayAnimation(HijackIndicatorPopup, 0, 1, EUMGSequencePlayMode::Reverse);
}

void UPlayerHUD::SetDamageNumber(float InDamage, const FLinearColor InColor)
{
	if (!DamageNumberText) return;
	if (InDamage < 1.f) InDamage = 1.f;
	DamageNumberText->SetColorAndOpacity(InColor);
	DamageNumberText->SetText(FText::Format(FText::FromString("-{0}"), FText::AsNumber(FMath::RoundToInt(InDamage))));
	PlayAnimation(DamageReceivedAnim);
}

void UPlayerHUD::CollapseAllAbilitiesPanels() const
{
	RangerAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
	TricksterAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
	GuardianAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
	BreacherAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
	WraithAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
	TankAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
	SpyAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
	SamuraiAbilitiesPanel->UnbindOnKeyBindChangeDelegate();
	RangerAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
	TricksterAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
	GuardianAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
	BreacherAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
	WraithAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
	TankAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
	SpyAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
	SamuraiAbilitiesPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerHUD::CollapseAllCrosshairs() const
{
	RangerCrosshair->SetVisibility(ESlateVisibility::Collapsed);
	TricksterCrosshair->SetVisibility(ESlateVisibility::Collapsed);
	GuardianCrosshair->SetVisibility(ESlateVisibility::Collapsed);
	BreacherCrosshair->SetVisibility(ESlateVisibility::Collapsed);
	WraithCrosshair->SetVisibility(ESlateVisibility::Collapsed);
	TankCrosshair->SetVisibility(ESlateVisibility::Collapsed);
	SpyCrosshair->SetVisibility(ESlateVisibility::Collapsed);
	SamuraiCrosshair->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerHUD::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(OutOfAmmoDisplayHandle);
	GetWorld()->GetTimerManager().ClearTimer(UpdateHpWhiteBarHandle);
	GetWorld()->GetTimerManager().ClearTimer(UpdateGlobalHpWhiteBarHandle);
	Super::NativeDestruct();
}

void UPlayerHUD::UpdateUpgradeAmountText(const int32 InAmount) const
{
	if (!UpgradeAmountText || !UpgradeTextInv) return;
	UpgradeAmountText->SetText(FText::AsNumber(InAmount));
	UpgradeTextInv->SetVisibility(InAmount > 0 ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UPlayerHUD::UpdateUpgradeKeyBindText(const FName& InKeyBindName, const FText& InKeyName) const
{
	if (!InKeyBindName.IsEqual("OpenUpgrade") || !UpgradeKeyBindText) return;
	UpgradeKeyBindText->SetText(InKeyName);
}

template<typename T>
void UPlayerHUD::ToggleUI(const TObjectPtr<T>& InUI)
{
	if (!InUI) return;
	InUI->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? InUI->SetVisibility(ESlateVisibility::Collapsed) : InUI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
