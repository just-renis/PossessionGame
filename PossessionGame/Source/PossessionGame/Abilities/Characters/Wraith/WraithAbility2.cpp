#include "WraithAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Wraith.h"

void UWraithAbility2::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	//Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::ThrowMine);
	Proprietor->P_InitiateThrowTrap();
	Super::P_Execute();
}

bool UWraithAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UWraithAbility2::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<AWraith>(InAbilityProprietor); }