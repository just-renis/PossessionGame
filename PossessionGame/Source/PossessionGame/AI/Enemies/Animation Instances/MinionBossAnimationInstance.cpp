#include "MinionBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Controllers/MinionBossAIController.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"

void UMinionBossAnimationInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* StompMontage = Montages.Find(EEnemyAnimState::Stomp);
	if (StompMontage && Montage == StompMontage->Get())
	{
		Proprietor->SetActionInMotion(false);
		SetStompIsPlaying(false);
		(void)Cast<AMinionBossAIController>(Proprietor->GetAI())->OnStompFinished.ExecuteIfBound();
	}
}
