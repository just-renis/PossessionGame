#include "STTaskShoot.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

EStateTreeRunStatus USTTaskShoot::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<AEnemyAIController>(Context.GetOwner());
	CachedController->GetStateTreeData().OnStateTreeEvent.BindUObject(this, &USTTaskShoot::FinishTaskByEvent);
	UKismetSystemLibrary::K2_SetTimer(this, "ShootLogic", CachedController->GetProprietor()->GetWeapon()->GetShootCooldown(), true);
	if (!CachedController->GetProprietor()->GetWeapon()->GetIsFirstShotWasShot())
	{
		CachedController->GetProprietor()->GetWeapon()->SetFirstShotWasShot();
		ShootLogic();
	}
	return Super::EnterState(Context, Transition);
}

void USTTaskShoot::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UKismetSystemLibrary::K2_ClearTimer(this, "ShootLogic");
	CachedController->GetProprietor()->AI_CancelShooting();
	CachedController->GetStateTreeData().OnStateTreeEvent.Unbind();
	Super::ExitState(Context, Transition);
}

void USTTaskShoot::FinishTaskByEvent(const EStateTreeEvent InEvent)
{
	UKismetSystemLibrary::K2_ClearTimer(this, "ShootLogic");
	CachedController->GetProprietor()->AI_CancelShooting();
	CachedController->GetStateTreeData().OnStateTreeEvent.Unbind();
	FinishTask();
}

void USTTaskShoot::ShootLogic()
{
	if (bIsWeaponMontageOnly ? !CachedController->GetProprietor()->GetWeapon()->GetAnimInstance()->IsPlaying(EWeaponAnimState::Shoot) : !CachedController->GetProprietor()->GetAnimInstance()->IsPlaying(EEnemyAnimState::Shoot)) CachedController->GetProprietor()->AI_InitiateShooting();
	CachedController->GetProprietor()->AI_ShootLogic();
}
