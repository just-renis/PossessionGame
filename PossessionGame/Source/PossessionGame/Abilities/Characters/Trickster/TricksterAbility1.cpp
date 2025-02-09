#include "TricksterAbility1.h"

#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UTricksterAbility1::P_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::GrenadeLaunch);
	UKismetSystemLibrary::K2_SetTimer(this, "ThrowSmokeGrenade", 0.4f, false);
	Super::P_Execute();
}

bool UTricksterAbility1::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void UTricksterAbility1::SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ATrickster>(AbilityProprietor); }

void UTricksterAbility1::ThrowSmokeGrenade() const { Proprietor->ThrowSmokeGrenade(); }
