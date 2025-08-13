#include "VariationUnlockDetailsUI.h"
#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

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

void UVariationUnlockDetailsUI::SetupDetailsPanel(const EVariation InVariation, const int32 InUnlockPrice)
{
	Variation = InVariation;
	UnlockPrice = InUnlockPrice;
	if (CostPriceText) CostPriceText->SetText(FText::AsNumber(UnlockPrice));
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
	UnlockButton->SetButtonVisibility(bState);
	UnlockButton->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	CostPriceBox->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UVariationUnlockDetailsUI::UnlockVariation()
{
	if (SaveSlot->GetCurrencyAmount() >= UnlockPrice)
	{
		SaveSlot->AddCurrencyAmount(-UnlockPrice);
		SaveSlot->UnlockVariation(Variation);
		SetUnlockedVariation(true);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnVariationUnlocked.Broadcast(Variation);
		SaveSlot->SaveSlot();
	}
}
