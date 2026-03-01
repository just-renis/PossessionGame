#include "TankHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void UTankHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EHandsAnimState::Throw, InMontage)) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
