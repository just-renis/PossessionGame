#include "TricksterAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Controllers/TricksterAIController.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"

void UTricksterAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (Proprietor->IsActionsDisabled() || !IsMontage(EEnemyAnimState::GrenadeLaunch, InMontage)) return;
	Proprietor->SetIsActionInMotion(false);
	(void)Cast<ATricksterAIController>(Proprietor->GetAI())->OnPoisonGrenadeLaunched.ExecuteIfBound();
}
