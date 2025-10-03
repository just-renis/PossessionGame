#include "PlayerDataManager.h"

void UPlayerDataManager::Reset()
{
	TricksterPurpleVariationData = FTricksterPurpleVariationData();
	RangerGrenadeSpecialData = FRangerGrenadeSpecialData();
	RangerShieldSpecialData = FRangerShieldSpecialData();
	GuardianTimeStopSpecialData = FGuardianTimeStopSpecialData();
	RangerBlueVariationData = FRangerBlueVariationData();
}

void UPlayerDataManager::Tick(const float InTickTime)
{
	TricksterPurpleVariationData.TickStackDuration(InTickTime);
	RangerGrenadeSpecialData.TickStackDuration(InTickTime);
	RangerShieldSpecialData.TickStackDuration(InTickTime);
	GuardianTimeStopSpecialData.TickStackDuration(InTickTime);
	RangerBlueVariationData.TickStackDuration(InTickTime);
}
