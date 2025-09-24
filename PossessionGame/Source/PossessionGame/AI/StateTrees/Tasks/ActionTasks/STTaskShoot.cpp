#include "STTaskShoot.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

EStateTreeRunStatus USTTaskShoot::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	CachedController = Cast<AEnemyAIController>(Context.GetOwner());
	CachedController->GetStateTreeData().OnStateTreeEvent.BindUObject(this, &USTTaskShoot::FinishTaskByEvent);
	GetWorld()->GetTimerManager().SetTimer(ShootLogicHandle, this, &USTTaskShoot::ShootLogic, CachedController->GetProprietor()->GetWeapon()->GetShootCooldown(), true);
	if (!CachedController->GetProprietor()->GetWeapon()->GetIsFirstShotWasShot())
	{
		CachedController->GetProprietor()->GetWeapon()->SetFirstShotWasShot();
		ShootLogic();
	}
	return Super::EnterState(Context, Transition);
}

void USTTaskShoot::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	GetWorld()->GetTimerManager().ClearTimer(ShootLogicHandle);
	CachedController->GetProprietor()->AI_CancelShooting();
	CachedController->GetStateTreeData().OnStateTreeEvent.Unbind();
	Super::ExitState(Context, Transition);
}

void USTTaskShoot::FinishTaskByEvent(const EStateTreeEvent InEvent)
{
	GetWorld()->GetTimerManager().ClearTimer(ShootLogicHandle);
	CachedController->GetProprietor()->AI_CancelShooting();
	CachedController->GetStateTreeData().OnStateTreeEvent.Unbind();
	FinishTask();
}

void USTTaskShoot::ShootLogic()
{
	UE_LOG(LogTemp, Error, TEXT("ShootLogic 1"));
	if (!CachedController->GetStateTreeData().GetIsTargetInMySight()) return;
	UE_LOG(LogTemp, Error, TEXT("ShootLogic 2"));
	const TObjectPtr<AEnemy> Enemy = CachedController->GetProprietor();
	if (bIsWeaponMontageOnly ? !Enemy->GetWeapon()->GetAnimInstance()->IsPlaying(EWeaponAnimState::Shoot) : !Enemy->GetAnimInstance()->IsPlaying(EEnemyAnimState::Shoot)) Enemy->AI_InitiateShooting();
	if (!Enemy->GetIsActionSpeedModified() || Enemy->GetWeapon()->AI_CanShoot()) Enemy->AI_ShootLogic();
}
