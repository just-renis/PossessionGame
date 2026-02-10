#include "PyroBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Controllers/PyroBossAIController.h"

void UPyroBossAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EEnemyAnimState::ActivateFloorLava, InMontage))
	{
		(void)Cast<APyroBossAIController>(Proprietor->GetAI())->OnActivateFireFloorFinished.ExecuteIfBound();
		if (!Proprietor->IsActionsDisabled()) Proprietor->SetIsActionInMotion(false);
		Proprietor->SetIsAbilityInAction(false);
	}
	if (!IsMontage(EEnemyAnimState::Stomp, InMontage)) return;
	(void)Cast<APyroBossAIController>(Proprietor->GetAI())->OnActivateStompFinished.ExecuteIfBound();
	if (!Proprietor->IsActionsDisabled()) Proprietor->SetIsActionInMotion(false);
	Proprietor->SetIsAbilityInAction(false);
	Proprietor->GetAnimInstance()->SetNonBlendableSlotState(false);
}
