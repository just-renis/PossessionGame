#include "SamuraiAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"

void USamuraiAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (!IsMontage(EEnemyAnimState::Slash, InMontage)) return;
	if (!Proprietor->IsActionsDisabled()) Proprietor->SetIsActionInMotion(false);
}
