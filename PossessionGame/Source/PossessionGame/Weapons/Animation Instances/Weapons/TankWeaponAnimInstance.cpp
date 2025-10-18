#include "TankWeaponAnimInstance.h"

void UTankWeaponAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	if (bIsInterrupted) return;
	const TObjectPtr<UAnimMontage>* ChargeUpMontage = Montages.Find(EWeaponAnimState::ChargeUp);
	if (ChargeUpMontage && Montage == ChargeUpMontage->Get()) PlayMontage(EWeaponAnimState::Shoot);
}
