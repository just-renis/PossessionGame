#include "MinionBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Characters/MinionBoss.h"
#include "PossessionGame/AI/Enemies/Controllers/MinionBossAIController.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"

void UMinionBossAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EEnemyAnimState::Stomp, InMontage))
	{
		(void)Cast<AMinionBossAIController>(Proprietor->GetAI())->OnStompFinished.ExecuteIfBound();
		SetIsActionMontagePlaying(false);
		Cast<AMinionBoss>(Proprietor)->OnStompFinish();
		return;
	}
	if (!IsMontage(EEnemyAnimState::StopLaser, InMontage)) return;
	(void)Cast<AMinionBossAIController>(Proprietor->GetAI())->OnLaserEyeFinished.ExecuteIfBound();
	Cast<AMinionBoss>(Proprietor)->OnLaserFinish();
	SetIsActionMontagePlaying(false);
}

void UMinionBossAnimationInstance::MontageWithHoldEnd()
{
	if (MontageStateInHold != EEnemyAnimState::ShootLaser) return;
	Proprietor->AI_StopAbility(1);
}
