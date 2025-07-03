#include "VariationUnlockDetailsUI.h"
#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void UVariationUnlockDetailsUI::SetDetailsText(const FText& InText) const
{
	if (!DetailsText) return;
	DetailsText->SetText(InText);
}

void UVariationUnlockDetailsUI::SetTaskCompleted(const bool bState) const
{
	if (!UnlockButton || !DetailsCheck) return;
	UnlockButton->SetIsEnabled(bState);
	DetailsCheck->SetIsChecked(bState);
	SetUnlockedVariation(false);
}

void UVariationUnlockDetailsUI::SetUnlockedVariation(const bool bState) const
{
	ChangeVisibilityOfUnlockElements(!bState);
	if (!DetailsText) return;
	if (bState) DetailsText->SetText(FText::FromString("UNLOCKED"));
	DetailsText->SetJustification(bState ? ETextJustify::Center : ETextJustify::Left);
	FSlateFontInfo FontInfo = DetailsText->GetFont();
	FontInfo.Size = bState ? 40 : 18;
	DetailsText->SetFont(FontInfo);
}

void UVariationUnlockDetailsUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!bIsCheckBoxNeeded)
	{
		ChangeVisibilityOfUnlockElements(false);
		if (UnlockButton) UnlockButton->SetIsEnabled(false);
	}
}

void UVariationUnlockDetailsUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (UnlockButton) UnlockButton->Get()->OnClicked.AddDynamic(this, &UVariationUnlockDetailsUI::UnlockVariation);
}

void UVariationUnlockDetailsUI::ChangeVisibilityOfUnlockElements(const bool bState) const
{
	if (!DetailsCheck || !UnlockButton || !CostPriceBox) return;
	DetailsCheck->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	UnlockButton->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	CostPriceBox->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UVariationUnlockDetailsUI::UnlockVariation()
{
	
	UE_LOG(LogTemp, Error, TEXT("UNLOCK THE VARIATION"));
}
