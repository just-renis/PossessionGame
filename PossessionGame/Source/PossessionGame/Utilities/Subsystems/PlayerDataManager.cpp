#include "PlayerDataManager.h"

void UPlayerDataManager::AddRangerGrenadeSpecialStack()
{
	RangerGrenadeSpecialData.AddStack();
	
}

void UPlayerDataManager::Reset()
{
	TricksterPurpleVariationData = FTricksterPurpleVariationData();
	RangerGrenadeSpecialData = FRangerGrenadeSpecialData();
}

void UPlayerDataManager::Tick(const float InTickTime)
{
	TricksterPurpleVariationData.TickStackDuration(InTickTime);
	RangerGrenadeSpecialData.TickStackDuration(InTickTime);
}
