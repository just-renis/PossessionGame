#include "TankHandsAnimInstance.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void UTankHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EHandsAnimState::Throw, InMontage)) (void)OnHandsMontageEnd.ExecuteIfBound(false);
	else if (IsMontage(EHandsAnimState::Slam, InMontage))
	{
		(void)OnHandsMontageEnd.ExecuteIfBound(false);
		GetProprietor()->SetInputState(false);
	}
}
