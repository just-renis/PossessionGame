#include "RangerAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void URangerAbility3::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::ThrowMine);
	Proprietor->P_InitiateThrowMine();
	Super::P_Execute();
}

bool URangerAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void URangerAbility3::SetProprietor(TObjectPtr<AEnemy> InAbilityProprietor) { Proprietor = Cast<ARanger>(InAbilityProprietor); }
