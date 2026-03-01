#include "TankAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Tank.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UTankAbility2::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::Throw);
	Proprietor->P_InitiateThrowGravitationalPull();
	Super::P_Execute();
}

bool UTankAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UTankAbility2::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ATank>(InAbilityProprietor); }
