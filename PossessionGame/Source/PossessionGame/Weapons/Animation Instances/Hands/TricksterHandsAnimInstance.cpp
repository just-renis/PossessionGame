#include "TricksterHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void UTricksterHandsAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* SmokeBombThrowMontage = Montages.Find(EHandsAnimState::GrenadeLaunch);
	if (SmokeBombThrowMontage && Montage == SmokeBombThrowMontage->Get())
	{
		(void)OnHandsMontageEnd.ExecuteIfBound(false);
		return;
	}
	const TObjectPtr<UAnimMontage>* EMPThrowMontage = Montages.Find(EHandsAnimState::SecondaryGrenadeLaunch);
	if (EMPThrowMontage && Montage == EMPThrowMontage->Get()) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
