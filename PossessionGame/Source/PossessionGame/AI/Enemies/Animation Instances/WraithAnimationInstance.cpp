#include "WraithAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Characters/Wraith.h"

void UWraithAnimationInstance::MontageWithHoldEnd()
{
	if (!Proprietor || Proprietor->GetIsHijacked()) return;
	if (MontageStateInHold != EEnemyAnimState::MeteorShower) return;
	StopMontage(EEnemyAnimState::MeteorShower, 0.1f);
	Cast<AWraith>(Proprietor)->AI_FinishMeteorShower();
}