#include "WraithAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Wraith.h"

bool UWraithAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UWraithAbility3::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<AWraith>(InAbilityProprietor); }
