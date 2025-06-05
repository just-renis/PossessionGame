#include "InfoAbilityDetailsUI.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInfoAbilityDetailsUI::SetAbilityIcon(const TObjectPtr<UTexture2D>& InAbilityIcon) const
{
	if (AbilityIcon) AbilityIcon->SetBrushFromAtlasInterface(InAbilityIcon);
	SetAbilityVisibility(true);
}

void UInfoAbilityDetailsUI::SetText(const FText& InText) const { if (AbilityExplanationText) AbilityExplanationText->SetText(InText); }

void UInfoAbilityDetailsUI::SetAbilityVisibility(const bool bState) const
{
	if (AbilityIcon) AbilityIcon->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}
