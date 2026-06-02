#include "VariationUnlockDetailsUI.h"
#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UVariationUnlockDetailsUI::SetDetailsText(const FText& InText) const
{
	if (!DetailsText) return;
	DetailsText->SetText(InText);
}

void UVariationUnlockDetailsUI::SetUnlockedVariation(const bool bState) const
{
	ChangeVisibilityOfUnlockElements(!bState);
	if (!DetailsText) return;
	if (bState) DetailsText->SetText(NSLOCTEXT("ST_Localization", "Unlocked", "UNLOCKED"));
}

void UVariationUnlockDetailsUI::SetupDetailsPanel(const EVariation InVariation, const int32 InUnlockPrice)
{
	Variation = InVariation;
	UnlockPrice = InUnlockPrice;
	if (CostPriceText) CostPriceText->SetText(FText::AsNumber(UnlockPrice));
}

void UVariationUnlockDetailsUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (UnlockButton) UnlockButton->Get()->OnClicked.AddDynamic(this, &UVariationUnlockDetailsUI::UnlockVariation);
}

void UVariationUnlockDetailsUI::ChangeVisibilityOfUnlockElements(const bool bState) const
{
	if (!UnlockButton || !CostPriceBox) return;
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
