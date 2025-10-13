#include "BreacherAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Breacher.h"

bool UBreacherAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UBreacherAbility1::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ABreacher>(AbilityProprietor); }
