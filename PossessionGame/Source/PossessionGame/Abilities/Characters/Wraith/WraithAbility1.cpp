#include "WraithAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Wraith.h"

bool UWraithAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UWraithAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<AWraith>(InAbilityProprietor); }
