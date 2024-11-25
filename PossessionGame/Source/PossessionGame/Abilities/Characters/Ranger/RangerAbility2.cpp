#include "RangerAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"

void URangerAbility2::AI_Execute()
{
	bIsAbilityInDuration = true;
	Proprietor->SpawnShield();
	Super::AI_Execute();
}

void URangerAbility2::P_Execute()
{
	bIsAbilityInDuration = true;
	Proprietor->SpawnShield();
	Super::P_Execute();
}

void URangerAbility2::P_StopAbility()
{
	if (bIsAbilityInDuration) Super::P_StopAbility();
}

bool URangerAbility2::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void URangerAbility2::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ARanger>(AbilityProprietor); }