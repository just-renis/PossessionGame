#include "PlayerLevelData.h"

float UPlayerLevelData::GetXpNeeded(const int32 InLevel)
{
	if (const float* FoundXpIncrease = LevelXpNeeded.Find(InLevel)) return *FoundXpIncrease;
	const int32 MaxStaticXpNeededLevel = LevelXpNeeded.Num() - 1;
	const float MaxNeededXp = *LevelXpNeeded.Find(MaxStaticXpNeededLevel);
	return MaxNeededXp + (InLevel - MaxStaticXpNeededLevel) * 50.f;
}
