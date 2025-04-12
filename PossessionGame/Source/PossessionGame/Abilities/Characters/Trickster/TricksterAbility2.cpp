#include "TricksterAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UTricksterAbility2::P_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::GrenadeLaunch);
	Proprietor->ThrowEmpGrenade();
	Super::P_Execute();
}

bool UTricksterAbility2::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void UTricksterAbility2::SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ATrickster>(AbilityProprietor); }
