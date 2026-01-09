#include "WraithHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void UWraithHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EHandsAnimState::Throw, InMontage)) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
