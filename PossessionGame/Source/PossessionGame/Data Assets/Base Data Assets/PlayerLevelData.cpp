#include "PlayerLevelData.h"

int32 UPlayerLevelData::GetDamageIncrease(const int32 InLevel)
{
	if (InLevel >= DamageIncreases.Num()) return *DamageIncreases.Find(DamageIncreases.Num());
	if (const int32* FoundDamageIncrease = DamageIncreases.Find(InLevel)) return *FoundDamageIncrease;
	return 0;
}

int32 UPlayerLevelData::GetHpIncrease(const int32 InLevel)
{
	if (InLevel >= HpIncreases.Num()) return *HpIncreases.Find(HpIncreases.Num());
	if (const int32* FoundHpIncrease = HpIncreases.Find(InLevel)) return *FoundHpIncrease;
	return 0;
}

int32 UPlayerLevelData::GetXpNeeded(const int32 InLevel)
{
	if (InLevel >= LevelXpNeeded.Num()) return *LevelXpNeeded.Find(LevelXpNeeded.Num());
	if (const int32* FoundXpIncrease = LevelXpNeeded.Find(InLevel)) return *FoundXpIncrease;
	return 1;
}
