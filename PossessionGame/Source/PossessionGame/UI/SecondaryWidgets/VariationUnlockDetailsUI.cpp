#include "VariationUnlockDetailsUI.h"
#include "Components/TextBlock.h"

void UVariationUnlockDetailsUI::SetUnlockText(const FText& InText) const
{
	if (!UnlockText) return;
	UnlockText->SetText(InText);
}

void UVariationUnlockDetailsUI::SetDetailsText(const FText& InText) const
{
	if (!DetailsText) return;
	DetailsText->SetText(InText);
}
