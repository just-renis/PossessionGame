#include "PlayerLevelData.h"

int32 UPlayerLevelData::GetDamageIncrease(const int32 InLevel)
{
	if (DamageIncreases.IsEmpty()) return 0;
	if (InLevel >= DamageIncreases.Num()) return DamageIncreases[DamageIncreases.Num() - 1];
	return DamageIncreases.Contains(InLevel) ? DamageIncreases[InLevel] : 0;
}

int32 UPlayerLevelData::GetHpIncrease(const int32 InLevel)
{
	if (HpIncreases.IsEmpty()) return 0;
	if (InLevel >= HpIncreases.Num()) return HpIncreases[HpIncreases.Num() - 1];
	return HpIncreases.Contains(InLevel) ? HpIncreases[InLevel] : 0;
}

int32 UPlayerLevelData::GetXpNeeded(const int32 InLevel)
{
	if (LevelXpNeeded.IsEmpty()) return 1;
	if (InLevel >= LevelXpNeeded.Num()) return LevelXpNeeded[LevelXpNeeded.Num() - 1];
	return LevelXpNeeded.Contains(InLevel) ? LevelXpNeeded[InLevel] : 1;
}
