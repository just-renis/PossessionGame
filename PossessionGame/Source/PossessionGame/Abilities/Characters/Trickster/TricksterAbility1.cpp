#include "TricksterAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UTricksterAbility1::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::SecondaryGrenadeLaunch);
	Proprietor->P_ThrowPoisonGrenade();
	Super::P_Execute();
}

bool UTricksterAbility1::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void UTricksterAbility1::SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ATrickster>(AbilityProprietor); }
