#include "UpgradeData.h"

FLevelUpgradeData UUpgradeData::GetAttributesForLevel(const bool bIsPlayer, const int32 InLevel) const
{
	if (const FLevelUpgradeData* FoundAttributes = bIsPlayer ? UpgradeAttributeValues.PlayerUpgrades.Find(InLevel) : UpgradeAttributeValues.AIUpgrades.Find(InLevel)) return *FoundAttributes;
	return {};
}
