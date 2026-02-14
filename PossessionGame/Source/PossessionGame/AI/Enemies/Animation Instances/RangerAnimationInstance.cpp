#include "RangerAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Controllers/RangerAIController.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void URangerAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (Proprietor->IsActionsDisabled()) return;
	if (IsMontage(EEnemyAnimState::GrenadeLaunch, InMontage))
	{
		Proprietor->SetIsActionInMotion(false);
		(void)Cast<ARangerAIController>(Proprietor->GetAI())->OnGrenadeLaunchFinished.ExecuteIfBound();
		return;
	}
	if (!IsMontage(EEnemyAnimState::DeployShield, InMontage)) return;
	Proprietor->SetIsActionInMotion(false);
	(void)Cast<ARangerAIController>(Proprietor->GetAI())->OnDeployShieldFinished.ExecuteIfBound();
}
