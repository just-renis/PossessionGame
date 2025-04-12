#include "FPlayerLevelData.h"

int32 FPlayerLevelData::GetDamageIncrease(const int32 InLevel)
{
	if (DamageIncreases.IsEmpty()) return 0;
	if (InLevel >= 10) return DamageIncreases[10];
	return DamageIncreases.Contains(InLevel) ? DamageIncreases[InLevel] : 0;
}

FPlayerLevelData::FPlayerLevelData()
{
	DamageIncreases.Add(1, 5);
	DamageIncreases.Add(2, 12);
	DamageIncreases.Add(3, 22);
	DamageIncreases.Add(4, 37);
	DamageIncreases.Add(5, 57);
	DamageIncreases.Add(6, 82);
	DamageIncreases.Add(7, 117);
	DamageIncreases.Add(8, 167);
	DamageIncreases.Add(9, 232);
	DamageIncreases.Add(10, 312);
}
