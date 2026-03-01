#include "BreacherHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void UBreacherHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (!IsMontage(EHandsAnimState::Throw, InMontage)) return;
	(void)OnHandsMontageEnd.ExecuteIfBound(false);
}
