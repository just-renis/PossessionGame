#include "MinionBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Controllers/MinionBossAIController.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"

void UMinionBossAnimationInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	UE_LOG(LogTemp, Warning, TEXT("MONTAGE NAME: %s %i"), *Montage->GetName(), bIsInterrupted);
	const TObjectPtr<UAnimMontage>* StompMontage = Montages.Find(EEnemyAnimState::Stomp);
	if (StompMontage && Montage == StompMontage->Get())
	{
		Proprietor->SetActionInMotion(false);
		SetStompIsPlaying(false);
		(void)Cast<AMinionBossAIController>(Proprietor->GetAI())->OnStompFinished.ExecuteIfBound();
		return;
	}
	const TObjectPtr<UAnimMontage>* PrepareLaserMontage = Montages.Find(EEnemyAnimState::PrepareLaser);
	if (PrepareLaserMontage && Montage == PrepareLaserMontage->Get())
	{
		Proprietor->GetAnimInstance()->PlayMontageWithHold(EEnemyAnimState::HoldLaser, 3.f, false);
		return;
	}
	const TObjectPtr<UAnimMontage>* StopLaserMontage = Montages.Find(EEnemyAnimState::StopLaser);
	if (StopLaserMontage && Montage == StopLaserMontage->Get())
	{
		Proprietor->SetActionInMotion(false);
		(void)Cast<AMinionBossAIController>(Proprietor->GetAI())->OnLaserEyeFinished.ExecuteIfBound();
	}
}
