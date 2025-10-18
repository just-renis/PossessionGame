#include "BreacherAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Breacher.h"

bool UBreacherAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UBreacherAbility2::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ABreacher>(AbilityProprietor); }
