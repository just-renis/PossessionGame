#include "SamuraiAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Samurai.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void USamuraiAbility1::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::DashSlash);
	Proprietor->P_DashSlash();
	Super::P_Execute();
}

bool USamuraiAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void USamuraiAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ASamurai>(InAbilityProprietor); }
