#include "MinionBossAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/MinionBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"

void UMinionBossAbility1::AI_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::Stomp);
	UE_LOG(LogTemp, Warning, TEXT("STOMP STARTED"));
	Super::AI_Execute();
}

bool UMinionBossAbility1::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void UMinionBossAbility1::SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AMinionBoss>(AbilityProprietor); }

void UMinionBossAbility1::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentCooldownSecs >= GetReqCooldownSecs())
	{
		UE_LOG(LogTemp, Warning, TEXT("TOGGLE STOMP TRIGGER - ABILITY CHARGED"));
		Proprietor->AI_ToggleStompTrigger(true);
	}
}
