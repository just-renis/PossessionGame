#include "PlayerHUD.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PossessionGame/UI/SecondaryWidgets/AbilitiesPanelUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

bool UPlayerHUD::GetPossessionIndicatorIsShowing() const { return PossessionIndicator ? PossessionIndicator->GetVisibility() == ESlateVisibility::SelfHitTestInvisible : false; }

void UPlayerHUD::UpdateProgressBar(const EUIBar InBar, const float InCurrent, const float InMax)
{
	switch (InBar)
	{
		case EUIBar::Hp:
			if (HpBar && HpWhiteBar && HpNumberText)
			{
				StepHpPercentage = (HpBar->GetPercent() - InCurrent / InMax) / 10.f;
				HpWhiteBar->SetPercent(HpBar->GetPercent());
				HpBar->SetPercent(InCurrent / InMax);
				HpNumberText->SetText(FText::FromString(FString::Format(TEXT("{0} / {1}"), {static_cast<int32>(InCurrent), static_cast<int32>(InMax)})));
				UKismetSystemLibrary::K2_SetTimer(this, "UpdateWhiteHpBar", 0.02f, true);
			}
			break;
		case EUIBar::Possession: if (PossessionBar) PossessionBar->SetPercent(InCurrent / InMax); break;
		case EUIBar::Xp:
			if (XpBar && XpNumberText)
			{
				XpNumberText->SetText(FText::FromString(FString::Format(TEXT("{0} / {1}"), {static_cast<int32>(InCurrent), static_cast<int32>(InMax)})));
				XpBar->SetPercent(InCurrent / InMax);;
			}
			break;
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

void UPlayerHUD::UpdatePossessionIndicator(const bool bState, const FString& InKeyName)
{
	if (PossessionIndicator) PossessionIndicator->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (PressKeyBindInv) PressKeyBindInv->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (PressKeyBindText) PressKeyBindText->SetText(FText::FromString("Press " + InKeyName + " to possess"));
	if (PossessionIndicatorOverlay)	PossessionIndicatorOverlay->SetVisibility(bState ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	if (bState)
	{
		GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ReadyToPossess);
		if (PossessionIndicatorPopup) PlayAnimation(PossessionIndicatorPopup);
		if (PressToPossessPopup) PlayAnimation(PressToPossessPopup);
	}
}

void UPlayerHUD::UpdateRunningIndicator(const bool bState) const { if (RunningIndicator) RunningIndicator->SetOpacity(bState ? 1.f : 0.3f); }

void UPlayerHUD::ToggleCriticalDamageIndicator() const
{
	if (!CriticalDamageIndicator) return;
	CriticalDamageIndicator->SetVisibility(CriticalDamageIndicator->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::ConfigureUIs()
{
	if (RunningIndicator) RunningIndicator->SetOpacity(0.3f);
	if (PressKeyBindInv) PressKeyBindInv->SetVisibility(ESlateVisibility::Collapsed);
	if (PossessionAnnouncementInv) PossessionAnnouncementInv->SetVisibility(ESlateVisibility::Collapsed);
	if (PossessionIndicator) PossessionIndicator->SetVisibility(ESlateVisibility::Collapsed);
	if (CriticalDamageIndicator) CriticalDamageIndicator->SetVisibility(ESlateVisibility::Collapsed);
	if (!GetAbilityPanel()) return;
	GetAbilityPanel()->CollapseAbilityIcon(0);
	GetAbilityPanel()->CollapseAbilityIcon(1);
	GetAbilityPanel()->CollapseAbilityIcon(2);
}

void UPlayerHUD::PlayDeathFadeOutAnim() { if (DeathFadeOut) PlayAnimation(DeathFadeOut); }

void UPlayerHUD::UpdateReceiveDamageInv()
{
	if (ReceiveDamageInv) ReceiveDamageInv->SetRenderOpacity(1.f);
	if (ReceiveDamageFadeOut) PlayAnimation(ReceiveDamageFadeOut);
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
}

void UPlayerHUD::AnnouncePossession()
{
	if (PossessionAnnouncementInv) PossessionAnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (PossessedPopup) PlayAnimation(PossessedPopup);
}

/*void UPlayerHUD::SetHitLocation(const FVector InHitLocation)
{
	const FVector Diff = (Player->GetActorLocation() - InHitLocation).GetSafeNormal(0.001f);
	const float Angle = FMath::Atan2(Player->GetActorForwardVector().X * Diff.Y - Player->GetActorForwardVector().Y * Diff.X, Player->GetActorForwardVector().Dot(Diff));
	SpawnHitDamageIndicator(Angle);
}*/

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
	const TObjectPtr<UDelegateManager> DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnPlayerDamageDealt.BindUObject(this, &UPlayerHUD::ShowDamageTick);
	DelegateManager->OnZoomToggle.BindUObject(this, &UPlayerHUD::ToggleCrosshair);
	DelegateManager->OnLevelUp.AddUObject(this, &UPlayerHUD::LevelUp);
}

void UPlayerHUD::ToggleCrosshair() { if (Crosshair) ToggleUI(Crosshair); }

/*void UPlayerHUD::SpawnHitDamageIndicator(const float InAngle)
{
	if (HitDamageIndicatorsPool.IsEmpty())
	{
		if (!WidgetTree) return;
		HitDamageIndicatorsPool.Push(WidgetTree->ConstructWidget<UImage>(UImage::StaticClass()));
		if (!HitDamageIndicator || HitDamageIndicatorsPool.IsEmpty()) return;
		HitDamageIndicator->AddChild(HitDamageIndicatorsPool[0]);
		const TObjectPtr<UImage> HitDamageIndicatorImg = HitDamageIndicatorsPool.Pop();
		const TObjectPtr<UMaterialInstanceDynamic> Material = UMaterialInstanceDynamic::Create(MI_HitDamageIndicator, this);
		Material->SetScalarParameterValue("Angle", GetHitDamageIndicatorSection(FMath::RadiansToDegrees(InAngle)));
		if (!HitDamageIndicatorImg || !HitDamageIndicatorImg->Slot) return;
		const TObjectPtr<UOverlaySlot> OverlaySlot = Cast<UOverlaySlot>(HitDamageIndicatorImg->Slot);
		if (!OverlaySlot) return;
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		HitDamageIndicatorImg->SetBrushFromMaterial(Material);
		InUseHitDamageIndicators.Push(FHitDamageIndicator(HitDamageIndicatorImg));
	}
	else
	{
		if (!HitDamageIndicator || HitDamageIndicatorsPool.IsEmpty()) return;
		HitDamageIndicator->AddChild(HitDamageIndicatorsPool[0]);
		const TObjectPtr<UImage> HitDamageIndicatorImg = HitDamageIndicatorsPool.Pop();
		if (!HitDamageIndicatorImg) return;
		HitDamageIndicatorImg->GetDynamicMaterial()->SetScalarParameterValue("Angle", GetHitDamageIndicatorSection(FMath::RadiansToDegrees(InAngle)));HitDamageIndicatorImg->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		InUseHitDamageIndicators.Push(FHitDamageIndicator(HitDamageIndicatorImg));
	}
}*/

void UPlayerHUD::ShowDamageTick(const bool bIsCritical)
{
	if (DamageTickInv) DamageTickInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (DamageTickImage && CriticalDamageTickTexture && DamageTickTexture) DamageTickImage->SetBrushFromTexture(bIsCritical ? CriticalDamageTickTexture : DamageTickTexture);
	if (DamageTickFadeOut) PlayAnimation(DamageTickFadeOut);
}

void UPlayerHUD::LevelUp(const int32 InLevel)
{
	PlayLevelUpAnim();
	SetLevelText(InLevel);
}

void UPlayerHUD::PlayLevelUpAnim()
{
	if (!LevelUpAnim || !LevelUpAnnouncementInv) return;
	LevelUpAnnouncementInv->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimation(LevelUpAnim);
}

void UPlayerHUD::SetLevelText(const int32 InLevel) const { if (LevelNumberText) LevelNumberText->SetText(FText::FromString(FString::FromInt(InLevel))); }

/*float UPlayerHUD::GetHitDamageIndicatorSection(const float InAngle)
{
	float SectionValue = 0.75f;
	if (InAngle >= -90.f && InAngle < 0.f) SectionValue = 0.5f;
	else if (InAngle >= 0.f && InAngle < 90.f) SectionValue = 0.25f;
	else if (InAngle >= 90.f && InAngle <= 180.f) SectionValue = 0.f;
	return SectionValue;
}*/

/*void UPlayerHUD::UpdateHitDamageIndicators()
{
	for (int32 i = InUseHitDamageIndicators.Num() - 1; i >= 0; i--)
	{
		if (InUseHitDamageIndicators[i].Image)
		{
			InUseHitDamageIndicators[i].CurrentDelay += 0.25f;
			if (InUseHitDamageIndicators[i].CurrentDelay >= InUseHitDamageIndicators[i].Delay)
			{
				InUseHitDamageIndicators[i].Image->SetVisibility(ESlateVisibility::Collapsed);
				HitDamageIndicatorsPool.Push(InUseHitDamageIndicators[i].Image);
				InUseHitDamageIndicators.RemoveAt(i);
			}
		}
	}
}*/

template<typename T>
void UPlayerHUD::ToggleUI(const TObjectPtr<T>& InUI)
{
	if (!InUI) return;
	InUI->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? InUI->SetVisibility(ESlateVisibility::Collapsed) : InUI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::UpdateWhiteHpBar()
{
	if (!HpWhiteBar || !HpBar) return;
	float NewPercentage = HpWhiteBar->GetPercent() - StepHpPercentage;
	if (NewPercentage <= HpBar->GetPercent())
	{
		NewPercentage = HpBar->GetPercent();
		UKismetSystemLibrary::K2_ClearTimer(this, "UpdateWhiteHpBar");
	}
	HpWhiteBar->SetPercent(NewPercentage);
}
