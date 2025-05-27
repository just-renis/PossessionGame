#include "FVariationUnlockData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Enums/EVariationUnlock.h"

FVariationUnlockData::FVariationUnlockData()
{
	SelectedVariations.Add(ECharacterType::Guardian, EVariation::GuardianVariationDefault);
	SelectedVariations.Add(ECharacterType::Ranger, EVariation::RangerVariationDefault);
	SelectedVariations.Add(ECharacterType::Trickster, EVariation::TricksterVariationDefault);
	VariationUnlocks.Add(EVariationUnlock::KillsWithGuardian, FVariationUnlockGoal(10));
	VariationUnlocks.Add(EVariationUnlock::KillsWithRangerGrenade, FVariationUnlockGoal(15));
	VariationUnlocks.Add(EVariationUnlock::DamageDealtWithTricksterPoisonGrenade, FVariationUnlockGoal(800));
}

bool FVariationUnlockData::IsVariationUnlocked(const EVariationUnlock InVariationUnlockName) const { return VariationUnlocks[InVariationUnlockName].Current >= VariationUnlocks[InVariationUnlockName].Requirement; }

int32 FVariationUnlockData::GetVariationUnlockProgress(const EVariationUnlock InVariationUnlockName) const { return VariationUnlocks[InVariationUnlockName].Current; }

int32 FVariationUnlockData::GetVariationUnlockRequirement(const EVariationUnlock InVariationUnlockName) const { return VariationUnlocks[InVariationUnlockName].Requirement; }

EVariation FVariationUnlockData::GetSelectedVariation(const ECharacterType InCharacter) const { return SelectedVariations[InCharacter]; }

void FVariationUnlockData::ProgressVariationUnlock(const EVariationUnlock InVariationUnlockName) { VariationUnlocks[InVariationUnlockName].Current++; }

void FVariationUnlockData::UpdateSelectedVariation(const ECharacterType InCharacterType, const EVariation InVariationName) { SelectedVariations[InCharacterType] = InVariationName; }
