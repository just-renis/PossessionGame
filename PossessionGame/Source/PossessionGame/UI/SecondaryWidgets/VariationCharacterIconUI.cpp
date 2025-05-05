#include "VariationCharacterIconUI.h"
#include "Components/Border.h"
#include "Components/Button.h"

void UVariationCharacterIconUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (VariationCharacterBorder && VariationCharacterIcon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(VariationCharacterIcon);
		VariationCharacterBorder->SetBrush(Brush);
	}
}

void UVariationCharacterIconUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (VariationCharacterButton) VariationCharacterButton->OnClicked.AddDynamic(this, &UVariationCharacterIconUI::HandleClicked);
}

void UVariationCharacterIconUI::HandleClicked() { (void)OnVariationClicked.ExecuteIfBound(VariationIndex); }
