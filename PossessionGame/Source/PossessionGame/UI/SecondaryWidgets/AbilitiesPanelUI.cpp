#include "AbilitiesPanelUI.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "PossessionGame/Enums/EUIBar.h"

void UAbilitiesPanelUI::SetOverlaysVisibility(const bool bState) const
{
	if (FirstAbilityOverlay) FirstAbilityOverlay->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (SecondAbilityOverlay) SecondAbilityOverlay->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (ThirdAbilityOverlay) ThirdAbilityOverlay->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UAbilitiesPanelUI::CollapseAbilityIcon(const int32 Index) const
{
	switch (Index)
	{
		case 0:
			if (!FirstAbilityImage || !FirstAbilityProgressBar) return;
			FirstAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
			FirstAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			FirstAbilityProgressBar->SetPercent(0.f);
			return;
		case 1:
			if (!SecondAbilityImage || !SecondAbilityProgressBar) return;
			SecondAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
			SecondAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SecondAbilityProgressBar->SetPercent(0.f);
			return;
		default:
			if (!ThirdAbilityImage || !ThirdAbilityProgressBar) return;
			ThirdAbilityImage->SetVisibility(ESlateVisibility::Collapsed);
			ThirdAbilityProgressBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			ThirdAbilityProgressBar->SetPercent(0.f);
	}
}

void UAbilitiesPanelUI::TriggerAbilityIconReady(const EUIBar InBar, const float InCurrent, const float InMax) const
{
	switch (InBar)
	{
		case EUIBar::FirstAbility: 
			if (!FirstAbilityImage || !FirstAbilityProgressBar) return;
			FirstAbilityProgressBar->SetPercent(InCurrent / InMax);
			if (InCurrent >= InMax)
			{
				FirstAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				FirstAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
			}
			break;
		case EUIBar::SecondAbility:
			if (!SecondAbilityImage || !SecondAbilityProgressBar) return;
			SecondAbilityProgressBar->SetPercent(InCurrent / InMax);
			if (InCurrent >= InMax)
			{
				SecondAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				SecondAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
			}
			break;
		default:
			if (!ThirdAbilityImage || !ThirdAbilityProgressBar) return;
			ThirdAbilityProgressBar->SetPercent(InCurrent / InMax);
			if (InCurrent >= InMax)
			{
				ThirdAbilityImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				ThirdAbilityProgressBar->SetVisibility(ESlateVisibility::Collapsed);
			}
	}
}

void UAbilitiesPanelUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (AbilityIcon1 && FirstAbilityImage) FirstAbilityImage->SetBrushFromAtlasInterface(AbilityIcon1);
	if (AbilityIcon2 && SecondAbilityImage) SecondAbilityImage->SetBrushFromAtlasInterface(AbilityIcon2);
	if (AbilityIcon3 && ThirdAbilityImage) ThirdAbilityImage->SetBrushFromAtlasInterface(AbilityIcon3);
}
