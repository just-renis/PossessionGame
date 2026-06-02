#include "VariationDescriptionUI.h"
#include "Components/TextBlock.h"

void UVariationDescriptionUI::SetDetailsText(const FText& InText) const
{
	if (!DetailsText) return;
	DetailsText->SetText(InText);
}