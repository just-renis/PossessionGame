#include "TankAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Tank.h"

bool UTankAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UTankAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ATank>(InAbilityProprietor); }
