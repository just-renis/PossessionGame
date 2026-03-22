#include "AcoBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AAcoBoss::AAcoBoss()
{
	AI_DestroyBossUnnecessaryComponents();
}

void AAcoBoss::AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	if (bHitByOtherAI) return;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), InDamage, GetActorLocation(), GetActorForwardVector());
	ReceiveDamage(InDamage, EDamageReason::BreacherBashDamage, nullptr, nullptr);
}

void AAcoBoss::BeginPlay()
{
	CharacterName = ECharacterType::AcoBoss;
	bIsBoss = true;
	Super::BeginPlay();
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	(void)DelegateManager->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::Boss);
	SpawnFirstPhaseEnemy();
	DelegateManager->OnAcoEnemySpawned.BindUObject(this, &AAcoBoss::SetupFlyingTowardsHijackTarget);
}

void AAcoBoss::ApplyEmpEffect(const float InEffectDuration) { Super::ApplyEmpEffect(InEffectDuration * 0.5f); }

void AAcoBoss::ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent)
{
	Super::ReceiveDamage(InDamage, InDamageReason, PointDamageEvent, RadialDamageEvent);
	(void)DelegateManager->UpdateGlobalHpBar.ExecuteIfBound(CurrentHp, MaxHp);
}

void AAcoBoss::InitiateDeath()
{
	Super::InitiateDeath();
	DelegateManager->OnBossDeath.Broadcast();
}

void AAcoBoss::SpawnFirstPhaseEnemy()
{
	const int32 Index = FMath::RandRange(0, CharactersToHijack.Num() - 1);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->RequestSpawnEnemy(CharactersToHijack[Index], EGameMode::FinalBossEnemySpawn, nullptr, nullptr);
	CharactersToHijack.RemoveAt(Index);
}

void AAcoBoss::SetupFlyingTowardsHijackTarget(const TObjectPtr<AActor>& InHijackTarget)
{
	HijackTargetEnemy = InHijackTarget;
	if (AI) AI->GetStateTreeData().SetEQSTargetRef(HijackTargetEnemy);
	GetWorldTimerManager().SetTimer(FlyingTowardsHijackEnemyHandle, this, &AAcoBoss::ProcessFlyingTowardsHijackTarget, 0.025f, true);
}

void AAcoBoss::ProcessFlyingTowardsHijackTarget()
{
	if (!HijackTargetEnemy) return;
	AddMovementInput((HijackTargetEnemy->GetActorLocation() - GetActorLocation()).GetSafeNormal(), MovementSpeedTowardsHijackTarget);
}
