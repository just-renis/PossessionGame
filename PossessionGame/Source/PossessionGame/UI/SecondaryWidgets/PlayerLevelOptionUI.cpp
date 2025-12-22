#include "PlayerLevelOptionUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void UPlayerLevelOptionUI::Setup(const TObjectPtr<UPlayerLevelOptionUI>& InOtherOptionWidget, const EPlayerLevelUpgradeId InId, const EPlayerLevelUpgradeId InOtherOptionId, const FText& InText, const bool bIsChosen)
{
	Id = InId;
	OtherOptionId = InOtherOptionId;
	if (!OptionText) return;
	OptionText->SetText(InText);
	if (!OptionButton) return;
	if (bIsChosen) SetAsChosen();
	OptionButton->OnClicked.AddDynamic(InOtherOptionWidget, &UPlayerLevelOptionUI::OnOtherOptionSelected);
}

void UPlayerLevelOptionUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!OptionButton) return;
	NotSelectedTexture = OptionButton->GetStyle().Normal.GetResourceObject();
	OptionButton->OnClicked.AddDynamic(this, &UPlayerLevelOptionUI::OnSelected);
}

void UPlayerLevelOptionUI::SetAsChosen() const
{
	FButtonStyle ButtonStyle = OptionButton->GetStyle();
	ButtonStyle.Normal.SetResourceObject(ButtonStyle.Disabled.GetResourceObject());
	OptionButton->SetStyle(ButtonStyle);
	OptionButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UPlayerLevelOptionUI::OnSelected()
{
	if (!OptionButton || OptionButton->GetVisibility() == ESlateVisibility::SelfHitTestInvisible) return;
	SetAsChosen();
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->ChangePlayerLevelUpgrade(Id, OtherOptionId);
}

void UPlayerLevelOptionUI::OnOtherOptionSelected()
{
	FButtonStyle ButtonStyle = OptionButton->GetStyle();
	ButtonStyle.Normal.SetResourceObject(NotSelectedTexture);
	OptionButton->SetStyle(ButtonStyle);
	OptionButton->SetVisibility(ESlateVisibility::Visible);
}
