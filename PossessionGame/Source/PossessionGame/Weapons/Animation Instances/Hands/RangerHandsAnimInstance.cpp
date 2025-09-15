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
	const TObjectPtr<UAnimMontage>* DeployShieldMontage = Montages.Find(EHandsAnimState::DeployShield);
	if (DeployShieldMontage && Montage == DeployShieldMontage->Get())
	{
		(void)OnHandsMontageEnd.ExecuteIfBound(false);
		return;
	}
	const TObjectPtr<UAnimMontage>* ThrowMineMontage = Montages.Find(EHandsAnimState::ThrowMine);
	if (ThrowMineMontage && Montage == ThrowMineMontage->Get()) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
