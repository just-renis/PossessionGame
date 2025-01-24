#include "PlayerHUD.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/InvalidationBox.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
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

void UPlayerHUD::SetHitLocation(const FVector InHitLocation)
{
	const FVector Diff = (Player->GetActorLocation() - InHitLocation).GetSafeNormal(0.001f);
	const float Angle = FMath::Atan2(Player->GetActorForwardVector().X * Diff.Y - Player->GetActorForwardVector().Y * Diff.X, Player->GetActorForwardVector().Dot(Diff));
	SpawnHitDamageIndicator(Angle);
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

void UPlayerHUD::SpawnHitDamageIndicator(const float InAngle)
{
	if (HitDamageIndicatorsPool.IsEmpty())
	{
		HitDamageIndicatorsPool.Push(WidgetTree->ConstructWidget<UImage>(UImage::StaticClass()));
		HitDamageIndicator->AddChild(HitDamageIndicatorsPool[0]);
		const TObjectPtr<UImage> HitDamageIndicatorImg = HitDamageIndicatorsPool.Pop();
		const TObjectPtr<UMaterialInstanceDynamic> Material = UMaterialInstanceDynamic::Create(MI_HitDamageIndicator, this);
		Material->SetScalarParameterValue("Angle", GetHitDamageIndicatorSection(FMath::RadiansToDegrees(InAngle)));
		const TObjectPtr<UOverlaySlot> OverlaySlot = Cast<UOverlaySlot>(HitDamageIndicatorImg->Slot);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		HitDamageIndicatorImg->SetBrushFromMaterial(Material);
		InUseHitDamageIndicators.Push(FHitDamageIndicator(HitDamageIndicatorImg));
	}
	else
	{
		HitDamageIndicator->AddChild(HitDamageIndicatorsPool[0]);
		const TObjectPtr<UImage> HitDamageIndicatorImg = HitDamageIndicatorsPool.Pop();
		HitDamageIndicatorImg->GetDynamicMaterial()->SetScalarParameterValue("Angle", GetHitDamageIndicatorSection(FMath::RadiansToDegrees(InAngle)));HitDamageIndicatorImg->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		InUseHitDamageIndicators.Push(FHitDamageIndicator(HitDamageIndicatorImg));
	}
}

float UPlayerHUD::GetHitDamageIndicatorSection(const float InAngle)
{
	float SectionValue = 0.75f;
	if (InAngle >= -90.f && InAngle < 0.f) SectionValue = 0.5f;
	else if (InAngle >= 0.f && InAngle < 90.f) SectionValue = 0.25f;
	else if (InAngle >= 90.f && InAngle <= 180.f) SectionValue = 0.f;
	return SectionValue;
}

void UPlayerHUD::UpdateHitDamageIndicators()
{
	for (int32 i = 0; i < InUseHitDamageIndicators.Num(); i++)
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

void UPlayerHUD::UpdateReceiveDamageInv()
{
	ReceiveDamageInv->SetRenderOpacity(1.f);
	PlayAnimation(ReceiveDamageFadeOut);
}

template<typename T>
void UPlayerHUD::ToggleUI(const TObjectPtr<T>& InUI)
{
	if (InUI) InUI->GetVisibility() == ESlateVisibility::SelfHitTestInvisible ? InUI->SetVisibility(ESlateVisibility::Collapsed) : InUI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
