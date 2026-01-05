#include "WraithAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Wraith.h"

void UWraithAbility1::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	//Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::ThrowMine);
	Proprietor->P_InitiateThrowMolotov();
	Super::P_Execute();
}

bool UWraithAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UWraithAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<AWraith>(InAbilityProprietor); }
