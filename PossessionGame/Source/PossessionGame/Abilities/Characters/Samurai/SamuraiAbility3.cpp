#include "SamuraiAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Samurai.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void USamuraiAbility3::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::ShurikenThrow);
	Proprietor->P_ThrowShuriken();
	Super::P_Execute();
}

bool USamuraiAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void USamuraiAbility3::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ASamurai>(InAbilityProprietor); }