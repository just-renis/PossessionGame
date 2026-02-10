#include "PyroBossAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/PyroBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"

void UPyroBossAbility1::AI_Execute()
{
	Proprietor->SetIsAbilityInAction(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::ActivateFloorLava);
	Super::AI_Execute();
}

bool UPyroBossAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UPyroBossAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<APyroBoss>(InAbilityProprietor); }

void UPyroBossAbility1::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentChargesAmount > 0) Proprietor->MarkFloorLavaAbilityReady();
}
