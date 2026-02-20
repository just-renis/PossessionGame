#include "SpyHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void USpyHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EHandsAnimState::TeleportImpact, InMontage)) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
