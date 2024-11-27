#include "PlayerHUD.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UPlayerHUD::UpdateProgressBar(EUIBar Bar, float Current, float Max)
{
	switch (Bar)
	{
		case EUIBar::Hp: if (HPBar) HPBar->SetPercent(Current / Max); break;
		default: if (PossessionBar) PossessionBar->SetPercent(Current / Max);
	}
}

void UPlayerHUD::UpdateAmmoText(int32 WeaponAmmo)
{
	if (AmmoText) AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d"), WeaponAmmo)));
}

void UPlayerHUD::UpdateAmmoText(int32 WeaponAmmo, int32 MaxAmmo)
{
	if (AmmoText) AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), WeaponAmmo, MaxAmmo)));
}

void UPlayerHUD::UpdateAmmoText(int32 FirstWeaponAmmo, int32 SecondWeaponAmmo, int32 MaxAmmo)
{
	if (AmmoText) AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d / %d"), FirstWeaponAmmo, SecondWeaponAmmo, MaxAmmo)));
}

void UPlayerHUD::ToggleAbilitiesUI()
{
	ToggleUI(FirstAbilityIcon);
	ToggleUI(SecondAbilityIcon);
	ToggleUI(ThirdAbilityIcon);
}

void UPlayerHUD::UpdateAbilityCooldown(int32 WhichAbility, float Secs)
{
	FText Text = Secs > 0.f ? FText::FromString(FString::Printf(TEXT("%.1f"), Secs)) : FText::FromString("USE");
	switch (WhichAbility)
	{
		case 0: FirstAbilityText->SetText(Text); break;
		case 1: SecondAbilityText->SetText(Text); break;
		default: ThirdAbilityText->SetText(Text);
	}
}

void UPlayerHUD::UpdateAbilityCooldown(int32 WhichAbility, int32 Kills)
{
	FText Text = Kills > 0 ? FText::FromString(FString::Printf(TEXT("%d"), Kills)) : FText::FromString("USE");
	switch (WhichAbility)
	{
		case 0: FirstAbilityText->SetText(Text); break;
		case 1: SecondAbilityText->SetText(Text); break;
		default: ThirdAbilityText->SetText(Text);
	}
}

void UPlayerHUD::ChangeColorOfText(int32 WhichAbility, FLinearColor Color)
{
	switch (WhichAbility)
	{
		case 0: FirstAbilityText->SetColorAndOpacity(Color); break;
		case 1: SecondAbilityText->SetColorAndOpacity(Color); break;
		default: ThirdAbilityText->SetColorAndOpacity(Color);
	}
}

void UPlayerHUD::UpdateFps(int32 Fps)
{
	FpsText->SetText(FText::FromString(FString::Printf(TEXT("FPS: %i"), Fps)));
}

void UPlayerHUD::BindCrosshairToggleDelegate()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnZoomToggle.BindUObject(this, &UPlayerHUD::ToggleCrosshair);
}

void UPlayerHUD::Reset()
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

void UPlayerHUD::SetColors(FLinearColor Color) { ScreenGlow->SetColorAndOpacity(Color); }

void UPlayerHUD::UpdatePossessionIndicator(const bool bState) const { PossessionIndicator->SetOpacity(bState ? 1.f : 0.3f); }

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	BindCrosshairToggleDelegate();
}

void UPlayerHUD::ToggleCrosshair() { ToggleUI(Crosshair); }

template<typename T>
void UPlayerHUD::ToggleUI(TObjectPtr<T> UI)
{
	if (UI) UI->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? UI->SetVisibility(ESlateVisibility::Collapsed) : UI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
