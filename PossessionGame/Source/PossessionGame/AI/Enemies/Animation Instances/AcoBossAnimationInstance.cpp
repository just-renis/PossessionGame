#include "AcoBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Characters/AcoBoss.h"

void UAcoBossAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EEnemyAnimState::StandUp, InMontage))
	{
		Cast<AAcoBoss>(Proprietor)->SpawnFirstPhaseEnemy();
		return;
	}
	if (!IsMontage(EEnemyAnimState::SlamFromSky, InMontage)) return;
	Cast<AAcoBoss>(Proprietor)->InitiateFight();
}
