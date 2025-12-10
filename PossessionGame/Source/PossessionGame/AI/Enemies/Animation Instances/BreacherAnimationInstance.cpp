#include "BreacherAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Controllers/BreacherAIController.h"

void UBreacherAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (!IsMontage(EEnemyAnimState::Bash, InMontage)) return;
	Proprietor->GetAI()->SetControlRigAvailabilityToggle(true);
	Proprietor->GetAnimInstance()->SetIsControlRigEnabled(true);
	(void)Cast<ABreacherAIController>(Proprietor->GetAI())->OnBashFinished.ExecuteIfBound();
	GetWorld()->GetTimerManager().SetTimer(BashMontagePlayingBlendOutHandle, this, &UBreacherAnimationInstance::SetNonBlendableSlotStateToFalse, 0.25f);
}
