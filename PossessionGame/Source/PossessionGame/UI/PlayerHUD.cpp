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
		default: if (PossessionBar) PossessionBar->SetPercent(InCurrent / InMax);
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

void UPlayerHUD::UpdateAbilityCooldown(const int32 InWhichAbility, const float InSecs) const
{
	const FText Text = InSecs > 0.f ? FText::FromString(FString::Printf(TEXT("%.1f"), InSecs)) : FText::FromString("USE");
	switch (InWhichAbility)
	{
		case 0: FirstAbilityText->SetText(Text); break;
		case 1: SecondAbilityText->SetText(Text); break;
		default: ThirdAbilityText->SetText(Text);
	}
}

void UPlayerHUD::UpdateAbilityCooldown(const int32 InWhichAbility, const int32 InKills) const
{
	const FText Text = InKills > 0 ? FText::FromString(FString::Printf(TEXT("%d"), InKills)) : FText::FromString("USE");
	switch (InWhichAbility)
	{
		case 0: FirstAbilityText->SetText(Text); break;
		case 1: SecondAbilityText->SetText(Text); break;
		default: ThirdAbilityText->SetText(Text);
	}
}

void UPlayerHUD::ChangeColorOfText(const int32 InWhichAbility, const FLinearColor InColor) const
{
	switch (InWhichAbility)
	{
		case 0: FirstAbilityText->SetColorAndOpacity(InColor); break;
		case 1: SecondAbilityText->SetColorAndOpacity(InColor); break;
		default: ThirdAbilityText->SetColorAndOpacity(InColor);
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
	FirstAbilityText->SetText(FText());
	SecondAbilityText->SetText(FText());
	ThirdAbilityText->SetText(FText());
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
}

void UPlayerHUD::PlayDeathFadeOutAnim() { PlayAnimation(DeathFadeOut); }

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	BindCrosshairToggleDelegate();
}

void UPlayerHUD::ToggleCrosshair() { ToggleUI(Crosshair); }

template<typename T>
void UPlayerHUD::ToggleUI(const TObjectPtr<T>& InUI)
{
	if (InUI) InUI->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? InUI->SetVisibility(ESlateVisibility::Collapsed) : InUI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
