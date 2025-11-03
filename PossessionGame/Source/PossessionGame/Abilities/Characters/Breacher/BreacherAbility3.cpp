#include "BreacherAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Breacher.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UBreacherAbility3::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::BreacherThrow);
	Proprietor->P_ThrowDynamite();
	Super::P_Execute();
}

bool UBreacherAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UBreacherAbility3::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ABreacher>(AbilityProprietor); }
