#include "BreacherAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Controllers/BreacherAIController.h"

void UBreacherAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* BashMontage = Montages.Find(EEnemyAnimState::Bash);
	if (BashMontage && InMontage == BashMontage->Get())
	{
		Proprietor->GetAI()->SetControlRigAvailabilityToggle(true);
		Proprietor->GetAnimInstance()->SetIsControlRigEnabled(true);
		(void)Cast<ABreacherAIController>(Proprietor->GetAI())->OnBashFinished.ExecuteIfBound();
		GetWorld()->GetTimerManager().SetTimer(BashMontagePlayingBlendOutHandle, this, &UBreacherAnimationInstance::SetIsBashMontagePlayingToFalse, 0.25f);
	}
}
