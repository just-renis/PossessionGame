#include "VariationCurrencyPanelUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UVariationCurrencyPanelUI::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerSaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	UpdateCurrencyText();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnCoinCollected.BindUObject(this, &UVariationCurrencyPanelUI::AddCurrency);
}

void UVariationCurrencyPanelUI::AddCurrency(const int32 InCurrencyAmount) const
{
	if (!PlayerSaveSlot) return;
	PlayerSaveSlot->AddCurrencyAmount(InCurrencyAmount);
	UpdateCurrencyText();
	PlayerSaveSlot->SaveSlot();
}

void UVariationCurrencyPanelUI::UpdateCurrencyText() const
{
	if (CurrencyText) CurrencyText->SetText(FText::AsNumber(PlayerSaveSlot->GetCurrencyAmount()));
}
