#include "FVariationUnlockData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EVariation.h"

FVariationUnlockData::FVariationUnlockData()
{
	SelectedVariations.Add(ECharacterType::Guardian, EVariation::GuardianVariationDefault);
	SelectedVariations.Add(ECharacterType::Ranger, EVariation::RangerVariationDefault);
	SelectedVariations.Add(ECharacterType::Trickster, EVariation::TricksterVariationDefault);
	VariationUnlocks.Add(EVariation::GuardianVariationGold, FVariationUnlockGoal(10));
	VariationUnlocks.Add(EVariation::RangerVariationBlue, FVariationUnlockGoal(15));
	VariationUnlocks.Add(EVariation::TricksterVariationPurple, FVariationUnlockGoal(100));
}

bool FVariationUnlockData::IsVariationUnlocked(const EVariation InVariationUnlockName)
{
	if (const FVariationUnlockGoal* VariationUnlockGoal = VariationUnlocks.Find(InVariationUnlockName))
	{
		return VariationUnlockGoal->bIsBought && VariationUnlockGoal->Current >= VariationUnlockGoal->Requirement;
	}
	return true;
}

bool FVariationUnlockData::IsVariationTaskCompleted(const EVariation InVariationUnlockName) const { return VariationUnlocks[InVariationUnlockName].Current >= VariationUnlocks[InVariationUnlockName].Requirement; }

int32 FVariationUnlockData::GetVariationUnlockProgress(const EVariation InVariationUnlockName) const { return VariationUnlocks[InVariationUnlockName].Current; }

int32 FVariationUnlockData::GetVariationUnlockRequirement(const EVariation InVariationUnlockName) const { return VariationUnlocks[InVariationUnlockName].Requirement; }

EVariation FVariationUnlockData::GetSelectedVariation(const ECharacterType InCharacter) const { return SelectedVariations[InCharacter]; }

void FVariationUnlockData::ProgressVariationUnlock(const EVariation InVariationUnlockName, const int32 InAmount) { VariationUnlocks[InVariationUnlockName].Current += InAmount; }

void FVariationUnlockData::UnlockVariation(const EVariation InVariationUnlockName) { VariationUnlocks[InVariationUnlockName].bIsBought = true; }

void FVariationUnlockData::UpdateSelectedVariation(const ECharacterType InCharacterType, const EVariation InVariationName) { SelectedVariations[InCharacterType] = InVariationName; }
