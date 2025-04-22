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
	UE_LOG(LogTemp, Warning, TEXT("EnterState"));
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
	UE_LOG(LogTemp, Warning, TEXT("ExitState"));
	GetWorld()->GetTimerManager().ClearTimer(ShootLogicHandle);
	CachedController->GetProprietor()->AI_CancelShooting();
	Super::ExitState(Context, Transition);
}

void USTTaskShoot::ShootLogic()
{
	UE_LOG(LogTemp, Warning, TEXT("TRY SHOOTING LMFAO ANFUINIFNIOR"));
	if (bIsWeaponMontageOnly ? !CachedController->GetProprietor()->GetWeapon()->GetAnimInstance()->IsPlaying(EWeaponAnimState::Shoot) : !CachedController->GetProprietor()->GetAnimInstance()->IsPlaying(EEnemyAnimState::Shoot)) CachedController->GetProprietor()->AI_InitiateShooting();
	CachedController->GetProprietor()->AI_ShootLogic();
}
