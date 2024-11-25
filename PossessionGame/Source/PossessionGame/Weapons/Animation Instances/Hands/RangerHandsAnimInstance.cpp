#include "RangerHandsAnimInstance.h"

void URangerHandsAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* GrenadeLaunchMontage = Montages.Find(EHandsAnimState::GrenadeLaunch);
	if (GrenadeLaunchMontage && Montage == GrenadeLaunchMontage->Get()) (void)OnGrenadeLaunchEnd.ExecuteIfBound(true);
}
