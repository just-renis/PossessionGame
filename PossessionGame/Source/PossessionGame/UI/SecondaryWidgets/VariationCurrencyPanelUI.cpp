#include "VariationCurrencyPanelUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UVariationCurrencyPanelUI::UpdateCurrencyText() const
{
	if (CurrencyText) CurrencyText->SetText(FText::AsNumber(PlayerSaveSlot->GetCurrencyAmount()));
}

void UVariationCurrencyPanelUI::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerSaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	UpdateCurrencyText();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnVariationUnlocked.AddUObject(this, &UVariationCurrencyPanelUI::UpdateCurrencyText);
}

void UVariationCurrencyPanelUI::UpdateCurrencyText(const EVariation InVariationName) const { UpdateCurrencyText(); }
