#include "PlayerLevelData.h"

int32 UPlayerLevelData::GetDamageIncrease(const int32 InLevel)
{
	if (DamageIncreases.IsEmpty()) return 0;
	if (InLevel >= 10) return DamageIncreases[10];
	return DamageIncreases.Contains(InLevel) ? DamageIncreases[InLevel] : 0;
}

int32 UPlayerLevelData::GetHpIncrease(const int32 InLevel)
{
	if (HpIncreases.IsEmpty()) return 0;
	if (InLevel >= 10) return HpIncreases[10];
	return HpIncreases.Contains(InLevel) ? HpIncreases[InLevel] : 0;
}
