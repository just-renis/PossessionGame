#include "PyroBossAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/PyroBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"

void UPyroBossAbility3::AI_Execute()
{
	Proprietor->SetIsAbilityInAction(true);
	Proprietor->GetAnimInstance()->PlayMontageWithHold(EEnemyAnimState::ActivatePyroBall, 6.f, false);
	Super::AI_Execute();
}

bool UPyroBossAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UPyroBossAbility3::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<APyroBoss>(InAbilityProprietor); }

void UPyroBossAbility3::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentChargesAmount > 0) Proprietor->MarkBallPhaseAbilityReady();
}
