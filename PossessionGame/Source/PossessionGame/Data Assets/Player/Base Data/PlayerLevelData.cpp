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

float UPlayerLevelData::GetXpNeeded(const int32 InLevel)
{
	if (const float* FoundXpIncrease = LevelXpNeeded.Find(InLevel)) return *FoundXpIncrease;
	const int32 MaxStaticXpNeededLevel = LevelXpNeeded.Num() - 1;
	const float MaxNeededXp = *LevelXpNeeded.Find(MaxStaticXpNeededLevel);
	return MaxNeededXp + (InLevel - MaxStaticXpNeededLevel) * 50.f;
}
