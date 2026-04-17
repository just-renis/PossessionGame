#include "PyroBossAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/PyroBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"

void UPyroBossAbility2::AI_Execute()
{
	Proprietor->SetIsAbilityInAction(true);
	Proprietor->GetAnimInstance()->SetNonBlendableSlotState(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::Stomp);
	Super::AI_Execute();
}

bool UPyroBossAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UPyroBossAbility2::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<APyroBoss>(InAbilityProprietor); }

void UPyroBossAbility2::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentChargesAmount > 0) Proprietor->MarkStompAbilityReady();
}
