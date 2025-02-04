#include "TricksterHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void UTricksterHandsAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* SmokeBombThrowMontage = Montages.Find(EHandsAnimState::GrenadeLaunch);
	if (SmokeBombThrowMontage && Montage == SmokeBombThrowMontage->Get()) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
