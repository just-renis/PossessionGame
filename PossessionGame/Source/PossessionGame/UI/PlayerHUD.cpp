#include "PlayerHUD.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UPlayerHUD::UpdateProgressBar(const EUIBar InBar, const float InCurrent, const float InMax) const
{
	switch (InBar)
	{
		case EUIBar::Hp: if (HPBar) HPBar->SetPercent(InCurrent / InMax); break;
		case EUIBar::Possession: if (PossessionBar) PossessionBar->SetPercent(InCurrent / InMax); break;
		default: TriggerAbilityIconReady(InBar, InCurrent, InMax);
	}
}

void UPlayerHUD::UpdateAmmoText(const int32 InWeaponAmmo) const
{
	if (AmmoText) AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d"), InWeaponAmmo)));
}

void UPlayerHUD::UpdateAmmoText(const int32 InWeaponAmmo, const int32 InMaxAmmo) const
{
	if (AmmoText) AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), InWeaponAmmo, InMaxAmmo)));
}

void UPlayerHUD::UpdateAmmoText(const int32 InFirstWeaponAmmo, const int32 InSecondWeaponAmmo, const int32 InMaxAmmo) const
{
	if (AmmoText) AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d / %d"), InFirstWeaponAmmo, InSecondWeaponAmmo, InMaxAmmo)));
}

void UPlayerHUD::ToggleAbilitiesUI()
{
	ToggleUI(FirstAbilityIcon);
	ToggleUI(SecondAbilityIcon);
	ToggleUI(ThirdAbilityIcon);
}

void UPlayerHUD::UpdateAbilityCooldown(const int32 InWhichAbility, const float InSecs, const float InReqSecs) const
{
	switch (InWhichAbility)
	{
		case 0: UpdateProgressBar(EUIBar::FirstAbility, InSecs, InReqSecs); break;
		case 1: UpdateProgressBar(EUIBar::SecondAbility, InSecs, InReqSecs); break;
		default: UpdateProgressBar(EUIBar::ThirdAbility, InSecs, InReqSecs);
	}
}

void UPlayerHUD::UpdateAbilityCooldown(const int32 InWhichAbility, const int32 InKills, const int32 InReqKills) const
{
	switch (InWhichAbility)
	{
		case 0: UpdateProgressBar(EUIBar::FirstAbility, InKills, InReqKills); break;
		case 1: UpdateProgressBar(EUIBar::SecondAbility, InKills, InReqKills); break;
		default: UpdateProgressBar(EUIBar::ThirdAbility, InKills, InReqKills);
	}
}

void UPlayerHUD::UpdateFps(const int32 InFps) const
{
	FpsText->SetText(FText::FromString(FString::Printf(TEXT("FPS: %i"), InFps)));
}

void UPlayerHUD::BindCrosshairToggleDelegate()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnZoomToggle.BindUObject(this, &UPlayerHUD::ToggleCrosshair);
}

void UPlayerHUD::Reset() const
{
	PossessionBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Crosshair->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AmmoText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	HPBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	FirstAbilityIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SecondAbilityIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ThirdAbilityIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::SetColors(const FLinearColor InColor) const { ScreenGlow->SetColorAndOpacity(InColor); }

void UPlayerHUD::UpdatePossessionIndicator(const bool bState) const { PossessionIndicator->SetOpacity(bState ? 1.f : 0.3f); }

void UPlayerHUD::UpdateRunningIndicator(const bool bState) const { RunningIndicator->SetOpacity(bState ? 1.f : 0.3f); }

void UPlayerHUD::ToggleCriticalDamageIndicator() const
{
	CriticalDamageIndicator->SetVisibility(CriticalDamageIndicator->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerHUD::ConfigureUIs() const
{
	PossessionIndicator->SetOpacity(0.3f);
	RunningIndicator->SetOpacity(0.3f);
	CriticalDamageIndicator->SetVisibility(ESlateVisibility::Collapsed);
	CollapseAbilityImage(0);
	CollapseAbilityImage(1);
	CollapseAbilityImage(2);
}

void UPlayerHUD::PlayDeathFadeOutAnim() { PlayAnimation(DeathFadeOut); }

void UPlayerHUD::CollapseAbilityImage(const int32 Index) const
{
	switch (Index)
	{
		case 0:
			FirstAbilityImage->SetVisibility(ESlateVisibility::Collapsed); 
			FirstAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			FirstAbilityProgressBar->SetPercent(0.f);
			break;
		case 1:
			SecondAbilityImage->SetVisibility(ESlateVisibility::Collapsed); 
			SecondAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SecondAbilityProgressBar->SetPercent(0.f);
			break;
		default:
			ThirdAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
			ThirdAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			ThirdAbilityProgressBar->SetPercent(0.f);
	}
}

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	BindCrosshairToggleDelegate();
}

void UPlayerHUD::ToggleCrosshair() { ToggleUI(Crosshair); }

void UPlayerHUD::TriggerAbilityIconReady(const EUIBar InBar, const float InCurrent, const float InMax) const
{
	switch (InBar)
	{
		case EUIBar::FirstAbility: 
			FirstAbilityProgressBar->SetPercent(InCurrent / InMax);
			if (FMath::IsNearlyEqual(InCurrent, InMax, 0.01f))
			{
				FirstAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				FirstAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
			}
			break;
		case EUIBar::SecondAbility: 
			SecondAbilityProgressBar->SetPercent(InCurrent / InMax);
			if (FMath::IsNearlyEqual(InCurrent, InMax, 0.01f))
			{
				SecondAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SecondAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
			}
			break;
		default:
			ThirdAbilityProgressBar->SetPercent(InCurrent / InMax);
			if (FMath::IsNearlyEqual(InCurrent, InMax, 0.01f))
			{
				ThirdAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				ThirdAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
			}
	}
}

template<typename T>
void UPlayerHUD::ToggleUI(const TObjectPtr<T>& InUI)
{
	if (InUI) InUI->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? InUI->SetVisibility(ESlateVisibility::Collapsed) : InUI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
