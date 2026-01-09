#include "WraithAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Wraith.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UWraithAbility1::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::Throw);
	Proprietor->P_InitiateThrowMolotov();
	Super::P_Execute();
}

bool UWraithAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UWraithAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<AWraith>(InAbilityProprietor); }
