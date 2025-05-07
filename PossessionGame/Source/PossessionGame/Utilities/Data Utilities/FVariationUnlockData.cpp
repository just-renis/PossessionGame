#include "FVariationUnlockData.h"
#include "PossessionGame/Enums/EVariationUnlock.h"

FVariationUnlockData::FVariationUnlockData()
{
	VariationUnlocks.Add(EVariationUnlock::KillsWithGuardian, FVariationUnlockGoal(5));
	VariationUnlocks.Add(EVariationUnlock::KillsWithRangerGrenade, FVariationUnlockGoal(20));
	VariationUnlocks.Add(EVariationUnlock::DamageDealtWithTricksterPoisonGrenade, FVariationUnlockGoal(500));
}

bool FVariationUnlockData::IsVariationUnlocked(const EVariationUnlock InVariationUnlockName) const
{
	return VariationUnlocks[InVariationUnlockName].Current >= VariationUnlocks[InVariationUnlockName].Requirement;
}

int32 FVariationUnlockData::GetVariationUnlockProgress(const EVariationUnlock InVariationUnlockName) const
{
	return VariationUnlocks[InVariationUnlockName].Current;
}

int32 FVariationUnlockData::GetVariationUnlockRequirement(const EVariationUnlock InVariationUnlockName) const
{
	return VariationUnlocks[InVariationUnlockName].Requirement;
}

void FVariationUnlockData::ProgressVariationUnlock(const EVariationUnlock InVariationUnlockName) { VariationUnlocks[InVariationUnlockName].Current++; }
