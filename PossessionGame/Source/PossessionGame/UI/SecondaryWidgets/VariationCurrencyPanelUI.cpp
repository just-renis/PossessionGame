#include "VariationCurrencyPanelUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void UVariationCurrencyPanelUI::UpdateCurrencyText() const
{
	if (CurrencyText) CurrencyText->SetText(FText::AsNumber(PlayerSaveSlot->GetCurrencyAmount()));
}

void UVariationCurrencyPanelUI::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerSaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot();
	UpdateCurrencyText();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnVariationUnlocked.AddUObject(this, &UVariationCurrencyPanelUI::UpdateCurrencyText);
}

void UVariationCurrencyPanelUI::UpdateCurrencyText(const EVariation InVariationName) const { UpdateCurrencyText(); }
