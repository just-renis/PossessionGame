#include "VariationOptionUI.h"
#include "Components/Border.h"

void UVariationOptionUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (VariationOptionBorder && VariationOptionIcon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(VariationOptionIcon);
		VariationOptionBorder->SetBrush(Brush);
	}
}
