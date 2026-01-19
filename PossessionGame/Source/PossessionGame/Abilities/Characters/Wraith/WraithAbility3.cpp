#include "WraithAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Wraith.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UWraithAbility3::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::Throw);
	Proprietor->P_InitiateThrowMeteorShower();
	Super::P_Execute();
}

bool UWraithAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UWraithAbility3::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<AWraith>(InAbilityProprietor); }
