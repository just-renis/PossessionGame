#include "WraithAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Controllers/WraithAIController.h"

void UWraithAnimationInstance::MontageWithHoldEnd()
{
	Super::MontageWithHoldEnd();
	if (MontageStateInHold != EEnemyAnimState::MeteorShower) return;
	Proprietor->GetAnimInstance()->SetIsControlRigEnabled(true);
	Proprietor->SetIsActionInMotion(false);
	(void)Cast<AWraithAIController>(Proprietor->GetAI())->OnMeteorShowerFinished.ExecuteIfBound();
	Proprietor->AI_StopAbility(2);
}