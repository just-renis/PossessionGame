#include "TricksterAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/EEnemyAnimState.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UTricksterAbility1::AI_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::GrenadeLaunch);
	Proprietor->PlaySound(ESound::RangerGrenadeLaunchAim, true);
	Super::AI_Execute();
}

void UTricksterAbility1::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::SecondaryGrenadeLaunch);
	Proprietor->P_ThrowPoisonGrenade();
	Super::P_Execute();
}

bool UTricksterAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UTricksterAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ATrickster>(InAbilityProprietor); }

void UTricksterAbility1::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (!Proprietor->GetIsHijacked() && CurrentChargesAmount > 0) Proprietor->AI_TogglePoisonGrenadeTrigger(true);
}
