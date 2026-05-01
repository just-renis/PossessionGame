#include "SamuraiAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Samurai.h"

void USamuraiAbility2::P_Execute()
{
	Proprietor->P_ChangeDeflectState(true);
	Super::P_Execute();
}

void USamuraiAbility2::P_StopAbility()
{
	if (!bIsAbilityInDuration) return;
	Proprietor->P_ChangeDeflectState(false);
	Super::P_StopAbility();
}

bool USamuraiAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void USamuraiAbility2::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ASamurai>(InAbilityProprietor); }
