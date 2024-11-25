#include "STTaskShoot.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

EStateTreeRunStatus USTTaskShoot::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<AEnemyAIController>(Context.GetOwner());
	CachedController->GetProprietor()->AI_InitiateShooting();
	UKismetSystemLibrary::K2_SetTimer(this, "ShootLogic", CachedController->GetProprietor()->GetWeapon()->GetShootCooldown(), true);
	return Super::EnterState(Context, Transition);
}

void USTTaskShoot::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UKismetSystemLibrary::K2_ClearTimer(this, "ShootLogic");
	Super::ExitState(Context, Transition);
}

void USTTaskShoot::ShootLogic()
{
	CachedController->GetProprietor()->AI_ShootLogic();
}
