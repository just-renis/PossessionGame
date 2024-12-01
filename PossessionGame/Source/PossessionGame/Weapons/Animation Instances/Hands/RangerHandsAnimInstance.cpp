#include "RangerHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void URangerHandsAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* GrenadeLaunchMontage = Montages.Find(EHandsAnimState::GrenadeLaunch);
	if (GrenadeLaunchMontage && Montage == GrenadeLaunchMontage->Get())
	{
		(void)OnHandsMontageEnd.ExecuteIfBound(false);
		return;
	}
	const TObjectPtr<UAnimMontage>* RangerDeployShieldMontage = Montages.Find(EHandsAnimState::RangerDeployShield);
	if (RangerDeployShieldMontage && Montage == RangerDeployShieldMontage->Get()) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
