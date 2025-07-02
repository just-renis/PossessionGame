#include "VariationUnlockDetailsUI.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

void UVariationUnlockDetailsUI::SetDetailsText(const FText& InText) const
{
	if (!DetailsText) return;
	DetailsText->SetText(InText);
}

void UVariationUnlockDetailsUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!bIsCheckBoxNeeded && DetailsCheck) DetailsCheck->SetVisibility(ESlateVisibility::Collapsed);
}
