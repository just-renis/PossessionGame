#include "RangerAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void URangerAbility1::AI_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::GrenadeLaunch);
	Proprietor->PlaySound(ESound::RangerGrenadeLaunchAim);
	Super::AI_Execute();
}

void URangerAbility1::P_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::GrenadeLaunch);
	Proprietor->LaunchGrenade();
	Super::P_Execute();
}

bool URangerAbility1::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void URangerAbility1::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ARanger>(AbilityProprietor); }

void URangerAbility1::CooldownLogic(const float DeltaTime)
{
	Super::CooldownLogic(DeltaTime);
	if (!Proprietor->GetIsPossessed() && CurrentCooldownSecs >= GetReqCooldownSecs()) Proprietor->AI_ToggleGrenadeLaunchTrigger(true);
}
