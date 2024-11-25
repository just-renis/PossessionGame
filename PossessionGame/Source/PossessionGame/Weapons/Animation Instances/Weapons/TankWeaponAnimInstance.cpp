#include "TankWeaponAnimInstance.h"

void UTankWeaponAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	if (bIsInterrupted) return;
	if (Montage == Montages.Find(EWeaponAnimState::ChargeUp)->Get()) PlayMontage(EWeaponAnimState::Shoot);
}
