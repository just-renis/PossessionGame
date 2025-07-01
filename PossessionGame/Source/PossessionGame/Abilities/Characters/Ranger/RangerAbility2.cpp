#include "RangerAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void URangerAbility2::AI_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::DeployShield);
	Proprietor->SpawnShield();
	Super::AI_Execute();
}

void URangerAbility2::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::RangerDeployShield);
	Proprietor->SpawnShield();
	Super::P_Execute();
}

bool URangerAbility2::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void URangerAbility2::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ARanger>(AbilityProprietor); }