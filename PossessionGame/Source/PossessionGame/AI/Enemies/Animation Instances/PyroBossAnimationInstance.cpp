#include "PyroBossAnimationInstance.h"

#include "PossessionGame/AI/Enemies/Characters/PyroBoss.h"
#include "PossessionGame/AI/Enemies/Controllers/PyroBossAIController.h"

void UPyroBossAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (!IsMontage(EEnemyAnimState::Stomp, InMontage)) return;
	(void)Cast<APyroBossAIController>(Proprietor->GetAI())->OnStompFinished.ExecuteIfBound();
	if (!Proprietor->IsActionsDisabled()) Proprietor->SetIsActionInMotion(false);
	Proprietor->SetIsAbilityInAction(false);
	Proprietor->GetAnimInstance()->SetNonBlendableSlotState(false);
}

void UPyroBossAnimationInstance::MontageWithHoldEnd()
{
	if (MontageStateInHold != EEnemyAnimState::ActivatePyroBall) return;
	(void)Cast<APyroBossAIController>(Proprietor->GetAI())->OnBallPhaseFinished.ExecuteIfBound();
	Cast<APyroBoss>(Proprietor)->FinishedBallPhase();
	if (!Proprietor->IsActionsDisabled()) Proprietor->SetIsActionInMotion(false);
	Proprietor->SetIsAbilityInAction(false);
}
