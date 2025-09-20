#include "GuardianHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void UGuardianHandsAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* OverChargeMontage = Montages.Find(EHandsAnimState::OverCharge);
	if (OverChargeMontage && Montage == OverChargeMontage->Get())
	{
		(void)OnHandsMontageEnd.ExecuteIfBound(false);
		return;
	}
	const TObjectPtr<UAnimMontage>* ShootMontage = Montages.Find(EHandsAnimState::Shoot);
	if (ShootMontage && Montage == ShootMontage->Get()) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
