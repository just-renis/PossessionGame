#include "VariationOptionUI.h"
#include "Components/Border.h"
#include "Components/Image.h"

void UVariationOptionUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!VariationOptionBorder || !VariationOptionIcon) return;
	if (!bIsLocked && LockedImage) LockedImage->SetVisibility(ESlateVisibility::Collapsed);
	FSlateBrush Brush;
	Brush.SetResourceObject(VariationOptionIcon);
	VariationOptionBorder->SetBrush(Brush);
}

void UVariationOptionUI::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (VariationOptionBorder) (void)ShowDetailsPanel.ExecuteIfBound(true, DetailsPanelText, UnlockText);
}

void UVariationOptionUI::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (VariationOptionBorder) (void)ShowDetailsPanel.ExecuteIfBound(false, FText(), FText());
}
