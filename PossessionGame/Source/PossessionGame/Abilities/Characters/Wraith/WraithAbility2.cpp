#include "WraithAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Wraith.h"

bool UWraithAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UWraithAbility2::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<AWraith>(InAbilityProprietor); }