#include "WraithAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Wraith.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UWraithAbility2::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::Throw);
	Proprietor->P_InitiateThrowTrap();
	Super::P_Execute();
}

bool UWraithAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UWraithAbility2::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<AWraith>(InAbilityProprietor); }