#include "BreacherHandsAnimInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"

void UBreacherHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* ThrowBarrelMontage = Montages.Find(EHandsAnimState::BreacherThrow);
	if (ThrowBarrelMontage && InMontage == ThrowBarrelMontage->Get()) (void)OnHandsMontageEnd.ExecuteIfBound(false);
}
