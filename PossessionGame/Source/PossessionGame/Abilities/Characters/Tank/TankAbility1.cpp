#include "TankAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Tank.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UTankAbility1::P_Execute()
{
	Proprietor->P_UseShield();
	Super::P_Execute();
}

void UTankAbility1::P_StopAbility()
{
	Proprietor->P_DisableShield();
	Super::P_StopAbility();
}

bool UTankAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UTankAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ATank>(InAbilityProprietor); }
