#include "TankAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Tank.h"

bool UTankAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UTankAbility2::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ATank>(AbilityProprietor); }
