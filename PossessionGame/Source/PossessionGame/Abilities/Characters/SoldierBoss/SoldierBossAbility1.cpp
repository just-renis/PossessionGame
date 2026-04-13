#include "SoldierBossAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/SoldierBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"

void USoldierBossAbility1::AI_Execute()
{
	Proprietor->SetIsAbilityInAction(true);
	Proprietor->GetAnimInstance()->SetNonBlendableSlotState(true);
	Proprietor->PlayMissilesLaunchReadyAnims(DurationTime);
	Super::AI_Execute();
}

bool USoldierBossAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void USoldierBossAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ASoldierBoss>(InAbilityProprietor); }

void USoldierBossAbility1::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentChargesAmount > 0) Proprietor->MarkMissilesLaunchAbilityReady();
}
