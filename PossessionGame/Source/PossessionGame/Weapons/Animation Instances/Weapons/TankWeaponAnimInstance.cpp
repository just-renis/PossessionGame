#include "TankWeaponAnimInstance.h"

void UTankWeaponAnimInstance::OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted)
{
	if (bIsInterrupted || !IsMontage(EWeaponAnimState::ChargeUp, InMontage)) return;
	PlayMontage(EWeaponAnimState::Shoot);
}
