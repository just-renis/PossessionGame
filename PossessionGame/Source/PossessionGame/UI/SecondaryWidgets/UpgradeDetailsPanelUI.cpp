#include "UpgradeDetailsPanelUI.h"
#include "Components/TextBlock.h"

void UUpgradeDetailsPanelUI::SetUpgradeText(const FString& InText) const
{
	if (!UpgradeText) return;
	UpgradeText->SetText(FText::FromString(InText));
}
