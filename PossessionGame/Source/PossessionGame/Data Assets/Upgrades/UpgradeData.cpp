#include "UpgradeData.h"

FUpgradeAttributes UUpgradeData::GetAttributesForLevel(const bool bIsPlayer, const int32 InLevel, const int32 InChoice) const
{
	if (bIsPlayer)
	{
		if (const FUpgradeChoices* LevelData = Upgrades.PlayerUpgrades.Find(InLevel))
		{
			if (const FUpgradeAttributes* FoundAttributes = LevelData->UpgradeChoices.Find(InChoice)) return *FoundAttributes;
		}
	}
	else if (const FUpgradeChoices* LevelData = Upgrades.AIUpgrades.Find(InLevel))
	{
		if (const FUpgradeAttributes* FoundAttributes = LevelData->UpgradeChoices.Find(InChoice)) return *FoundAttributes;
	}
	return {};
}
