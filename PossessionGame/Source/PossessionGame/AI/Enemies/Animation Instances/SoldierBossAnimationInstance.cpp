#include "SoldierBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Characters/SoldierBoss.h"
#include "PossessionGame/AI/Enemies/Controllers/SoldierBossAIController.h"

void USoldierBossAnimationInstance::MontageWithHoldEnd()
{
	if (MontageStateInHold != EEnemyAnimState::MissilesLaunch) return;
	(void)Cast<ASoldierBossAIController>(Proprietor->GetAI())->OnMissilesLaunchFinished.ExecuteIfBound();
	Cast<ASoldierBoss>(Proprietor)->FinishedMissilesLaunch();
	if (!Proprietor->IsActionsDisabled()) Proprietor->SetIsActionInMotion(false);
	Proprietor->SetIsAbilityInAction(false);
	Proprietor->GetAnimInstance()->SetNonBlendableSlotState(false);
}
