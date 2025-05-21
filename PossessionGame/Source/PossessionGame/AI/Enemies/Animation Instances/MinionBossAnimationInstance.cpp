#include "MinionBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Characters/MinionBoss.h"
#include "PossessionGame/AI/Enemies/Controllers/MinionBossAIController.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"

void UMinionBossAnimationInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* StompMontage = Montages.Find(EEnemyAnimState::Stomp);
	if (StompMontage && Montage == StompMontage->Get())
	{
		(void)Cast<AMinionBossAIController>(Proprietor->GetAI())->OnStompFinished.ExecuteIfBound();
		Proprietor->SetActionInMotion(false);
		Proprietor->SetAbilityInAction(false);
		SetIsActionMontagePlaying(false);
		return;
	}
	const TObjectPtr<UAnimMontage>* StopLaserMontage = Montages.Find(EEnemyAnimState::StopLaser);
	if (StopLaserMontage && Montage == StopLaserMontage->Get())
	{
		(void)Cast<AMinionBossAIController>(Proprietor->GetAI())->OnLaserEyeFinished.ExecuteIfBound();
		Cast<AMinionBoss>(Proprietor)->OnLaserFinish();
		SetIsActionMontagePlaying(false);
	}
}

void UMinionBossAnimationInstance::MontageWithHoldEnd()
{
	switch (MontageStateInHold)
	{
		case EEnemyAnimState::ShootLaser: Proprietor->AI_StopAbility(1); break;
		default: Super::MontageWithHoldEnd();
	}
}
