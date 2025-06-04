#include "PlayerLevelData.h"

int32 UPlayerLevelData::GetDamageIncrease(const int32 InLevel)
{
	if (DamageIncreases.IsEmpty()) return 0;
	if (InLevel >= DamageIncreases.Num()) return DamageIncreases[DamageIncreases.Num() - 1];
	if (const int32* FoundDamageIncrease = DamageIncreases.Find(InLevel)) return *FoundDamageIncrease;
	return 0;
}

int32 UPlayerLevelData::GetHpIncrease(const int32 InLevel)
{
	if (HpIncreases.IsEmpty()) return 0;
	if (InLevel >= HpIncreases.Num()) return HpIncreases[HpIncreases.Num() - 1];
	if (const int32* FoundHpIncrease = HpIncreases.Find(InLevel)) return *FoundHpIncrease;
	return 0;
}

int32 UPlayerLevelData::GetXpNeeded(const int32 InLevel)
{
	if (LevelXpNeeded.IsEmpty()) return 1;
	if (InLevel >= LevelXpNeeded.Num()) return LevelXpNeeded[LevelXpNeeded.Num() - 1];
	if (const int32* FoundXpIncrease = LevelXpNeeded.Find(InLevel)) return *FoundXpIncrease;
	return 1;
}
