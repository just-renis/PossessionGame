#include "PlayerDataManager.h"

void UPlayerDataManager::AddRangerGrenadeSpecialStack() { RangerGrenadeSpecialData.AddStack(); }

void UPlayerDataManager::AddRangerShieldSpecialStack() { RangerShieldSpecialData.AddStack(); }

void UPlayerDataManager::AddGuardianTimeStopSpecialStack() { GuardianTimeStopSpecialData.AddStack(); }

void UPlayerDataManager::Reset()
{
	TricksterPurpleVariationData = FTricksterPurpleVariationData();
	RangerGrenadeSpecialData = FRangerGrenadeSpecialData();
	RangerShieldSpecialData = FRangerShieldSpecialData();
	GuardianTimeStopSpecialData = FGuardianTimeStopSpecialData();
}

void UPlayerDataManager::Tick(const float InTickTime)
{
	TricksterPurpleVariationData.TickStackDuration(InTickTime);
	RangerGrenadeSpecialData.TickStackDuration(InTickTime);
	RangerShieldSpecialData.TickStackDuration(InTickTime);
	GuardianTimeStopSpecialData.TickStackDuration(InTickTime);
}
