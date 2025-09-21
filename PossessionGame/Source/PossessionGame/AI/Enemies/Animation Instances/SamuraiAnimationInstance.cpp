#include "SamuraiAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void USamuraiAnimationInstance::AssignOnMontageEnd()
{
	Super::AssignOnMontageEnd();
	OnMontageBlendingOut.AddDynamic(this, &USamuraiAnimationInstance::OnSamuraiMontageEnd);
}

void USamuraiAnimationInstance::OnSamuraiMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	if (bIsInterrupted) return;
	Proprietor->GetWeapon()->AI_Shoot();
}
