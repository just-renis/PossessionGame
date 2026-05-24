#include "FVariationUnlockData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EVariation.h"

FVariationUnlockData::FVariationUnlockData()
{
	SelectedVariations.Add(ECharacterType::Guardian, FUnlockedVariations({EVariation::GuardianDefault}, EVariation::GuardianDefault));
	SelectedVariations.Add(ECharacterType::Ranger, FUnlockedVariations({EVariation::RangerDefault}, EVariation::RangerDefault));
	SelectedVariations.Add(ECharacterType::Trickster, FUnlockedVariations({EVariation::TricksterDefault}, EVariation::TricksterDefault));
	SelectedVariations.Add(ECharacterType::Breacher, FUnlockedVariations({EVariation::BreacherDefault}, EVariation::BreacherDefault));
	SelectedVariations.Add(ECharacterType::Wraith, FUnlockedVariations({EVariation::WraithDefault}, EVariation::WraithDefault));
	SelectedVariations.Add(ECharacterType::Tank, FUnlockedVariations({EVariation::TankDefault}, EVariation::TankDefault));
	SelectedVariations.Add(ECharacterType::Spy, FUnlockedVariations({EVariation::SpyDefault}, EVariation::SpyDefault));
	SelectedVariations.Add(ECharacterType::Samurai, FUnlockedVariations({EVariation::SamuraiDefault}, EVariation::SamuraiDefault));
}

bool FVariationUnlockData::IsVariationUnlocked(const EVariation InVariationUnlockName)
{
	ECharacterType CharacterType;
	switch (InVariationUnlockName)
	{
		case EVariation::GuardianGreen: case EVariation::GuardianPurple: case EVariation::GuardianBlack: CharacterType = ECharacterType::Guardian; break;
		case EVariation::RangerBlue: case EVariation::RangerGreen: case EVariation::RangerBlack: CharacterType = ECharacterType::Ranger; break;
		case EVariation::TricksterPurple: case EVariation::TricksterLime: case EVariation::TricksterBlack: CharacterType = ECharacterType::Trickster; break;
		case EVariation::WraithBlack: case EVariation::WraithBlue: case EVariation::WraithPurple: CharacterType = ECharacterType::Wraith; break;
		case EVariation::BreacherBlack: case EVariation::BreacherRed: case EVariation::BreacherGreen: CharacterType = ECharacterType::Breacher; break;
		case EVariation::SpyOrange: case EVariation::SpyYellow: case EVariation::SpyBlack: CharacterType = ECharacterType::Spy; break;
		case EVariation::TankGreen: case EVariation::TankOrange: case EVariation::TankBlack: CharacterType = ECharacterType::Tank; break;
		case EVariation::SamuraiBlue: case EVariation::SamuraiRed: case EVariation::SamuraiBlack: CharacterType = ECharacterType::Samurai; break;
		default: return true;
	}
	return SelectedVariations[CharacterType].UnlockedVariations.Contains(InVariationUnlockName);
}

EVariation FVariationUnlockData::GetSelectedVariation(const ECharacterType InCharacter) const { return SelectedVariations[InCharacter].Current; }

void FVariationUnlockData::UpdateSelectedVariation(const ECharacterType InCharacterType, const EVariation InVariationName) { SelectedVariations[InCharacterType].Current = InVariationName; }

void FVariationUnlockData::UnlockVariation(const EVariation InVariationName)
{
	ECharacterType CharacterType;
	switch (InVariationName)
	{
		case EVariation::GuardianGreen: case EVariation::GuardianPurple: case EVariation::GuardianBlack: CharacterType = ECharacterType::Guardian; break;
		case EVariation::RangerBlue: case EVariation::RangerGreen: case EVariation::RangerBlack: CharacterType = ECharacterType::Ranger; break;
		case EVariation::TricksterPurple: case EVariation::TricksterLime: case EVariation::TricksterBlack: CharacterType = ECharacterType::Trickster; break;
		case EVariation::WraithBlack: case EVariation::WraithBlue: case EVariation::WraithPurple: CharacterType = ECharacterType::Wraith; break;
		case EVariation::BreacherBlack: case EVariation::BreacherRed: case EVariation::BreacherGreen: CharacterType = ECharacterType::Breacher; break;
		case EVariation::SpyOrange: case EVariation::SpyYellow: case EVariation::SpyBlack: CharacterType = ECharacterType::Spy; break;
		case EVariation::TankGreen: case EVariation::TankOrange: case EVariation::TankBlack: CharacterType = ECharacterType::Tank; break;
		case EVariation::SamuraiBlue: case EVariation::SamuraiRed: case EVariation::SamuraiBlack: CharacterType = ECharacterType::Samurai; break;
		default: return;
	}
	SelectedVariations[CharacterType].UnlockedVariations.Add(InVariationName);
}
