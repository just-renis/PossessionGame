#include "GuardianHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void UGuardianHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EHandsAnimState::Shoot, InMontage) || IsMontage(EHandsAnimState::OverCharge, InMontage)) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
