#include "VariationUnlockDetailsUI.h"
#include "Components/TextBlock.h"

void UVariationUnlockDetailsUI::SetDetailsText(const FText& InText) const
{
	if (!DetailsText) return;
	DetailsText->SetText(InText);
}
