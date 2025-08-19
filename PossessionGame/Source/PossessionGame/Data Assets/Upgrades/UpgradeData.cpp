#include "UpgradeData.h"

int32 UUpgradeData::GetMaxRarityChoicesCount(const EUpgradeChoice InUpgradeChoice)
{
	if (const FUpgradeChoices* UpgradeChoices = Upgrades.UpgradesChoices.Find(InUpgradeChoice)) return UpgradeChoices->UpgradeChoicesPairs.Num();
	return 0;
}

FUpgradeChoicePair UUpgradeData::GetRandomUpgradeChoice(const EUpgradeChoice InUpgradeChoice, const TArray<FUpgradeChoicePair>& OccupiedUpgradeChoices) { return Upgrades.GetRandomUpgradeChoice(InUpgradeChoice, OccupiedUpgradeChoices); }

FUpgradeChoices* UUpgradeData::GetUpgradeChoices(const EUpgradeChoice InUpgradeChoice)
{
	if (FUpgradeChoices* UpgradeChoices = Upgrades.UpgradesChoices.Find(InUpgradeChoice)) return UpgradeChoices;
	return nullptr;
}
