#include "RangerHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void URangerHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EHandsAnimState::GrenadeLaunch, InMontage) || IsMontage(EHandsAnimState::DeployShield, InMontage) || IsMontage(EHandsAnimState::ThrowMine, InMontage)) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
