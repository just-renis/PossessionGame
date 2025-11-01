#include "BreacherAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Breacher.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UBreacherAbility2::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontageWithHold(EHandsAnimState::BreacherBash, -1.f);
	Proprietor->P_Bash();
	Super::P_Execute();
}

bool UBreacherAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UBreacherAbility2::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ABreacher>(AbilityProprietor); }
