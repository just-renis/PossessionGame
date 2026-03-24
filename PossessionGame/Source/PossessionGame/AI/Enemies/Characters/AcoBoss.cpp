#include "AcoBoss.h"

#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PossessionGame/AI/Enemies/Animation Instances/AcoBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Enums/ECameraShake.h"
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
	DefaultSpawnLocation = GetActorLocation();
	Super::BeginPlay();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AcoAnimInstance = Cast<UAcoBossAnimationInstance>(GetAnimInstance());
	AcoAnimInstance->PlayMontage(EEnemyAnimState::Kneel);
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	(void)DelegateManager->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::Boss);
	DelegateManager->OnAcoEnemySpawned.BindUObject(this, &AAcoBoss::SetupFlyingTowardsHijackTarget);
	DelegateManager->OnFinalBossModeInitiated.BindUObject(this, &AAcoBoss::StandUp);
	DelegateManager->OnAcoEnemyDeath.BindUObject(this, &AAcoBoss::SetupFlyingTowardsColumnLocation);
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

FVector AAcoBoss::GetCurrentTargetLocation() const
{
	if (!HijackTargetEnemy) return DefaultSpawnLocation;
	return bIsFlyingTowardsHijackTarget ? HijackTargetEnemy->GetActorLocation() : DefaultSpawnLocation;
}

void AAcoBoss::SpawnFirstPhaseEnemy()
{
	if (CharactersToHijack.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("FIRST PHASE COMPLETED"));
		(void)DelegateManager->OnAcoFirstPhaseFinish.ExecuteIfBound();
		AcoAnimInstance->StopMontage(EEnemyAnimState::Kneel);
		GetWorldTimerManager().SetTimer(RiseUpwardsHandle, this, &AAcoBoss::RiseUpwards, 0.025f, true);
		return;
	}
	const int32 Index = FMath::RandRange(0, CharactersToHijack.Num() - 1);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->RequestSpawnEnemy(CharactersToHijack[Index], EGameMode::FinalBossEnemySpawn, nullptr, nullptr);
	CharactersToHijack.RemoveAt(Index);
}

void AAcoBoss::SetupFlyingTowardsHijackTarget(const TObjectPtr<AEnemy>& InHijackTarget)
{
	AcoAnimInstance->StopMontage(EEnemyAnimState::Kneel);
	if (!FlyTrailVfx) AssetsManager->AttachVfx(EVfx::AcoFlyTrail, nullptr, &FlyTrailVfx, GetMesh(), FName("RootSocket"));
	else FlyTrailVfx->Activate();
	bIsFlyingTowardsHijackTarget = true;
	HijackTargetEnemy = InHijackTarget;
	AcoAnimInstance->SetIsFlying(true);
	(void)DelegateManager->RequestTriggerCameraShake.ExecuteIfBound(ECameraShake::AcoDashInitiationCameraShake);
	GetWorldTimerManager().SetTimer(FlyingTowardsTargetHandle, this, &AAcoBoss::ProcessFlyingTowardsTarget, 0.025f, true);
}

void AAcoBoss::ProcessFlyingTowardsTarget()
{
	if (!HijackTargetEnemy) return;
	const FVector TargetLocation = GetCurrentTargetLocation();
	const FVector NewLocation = FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, 0.025f, MovementSpeedTowardsHijackTarget);
	SetActorLocation(NewLocation);
	if (FVector::Dist(NewLocation, TargetLocation) <= StopDistance) ReachedHijackTarget();
}

void AAcoBoss::ReachedHijackTarget()
{
	if (FlyTrailVfx) FlyTrailVfx->DeactivateImmediate();
	AcoAnimInstance->SetIsFlying(false);
	GetWorldTimerManager().ClearTimer(FlyingTowardsTargetHandle);
	if (GetMovementComponent()) GetMovementComponent()->StopMovementImmediately();
	if (bIsFlyingTowardsHijackTarget)
	{
		HijackTargetEnemy->AI_ApplyAcoHijack();
		ChangeActorRenderState(false);
	}
	else
	{
		AcoAnimInstance->PlayMontage(EEnemyAnimState::Kneel);
		FTimerHandle SpawnNextHijackAbleEnemyHandle;
		GetWorldTimerManager().SetTimer(SpawnNextHijackAbleEnemyHandle, this, &AAcoBoss::SpawnFirstPhaseEnemy, 1.f);
	}
}

void AAcoBoss::ChangeActorRenderState(const bool bState)
{
	SetActorTickEnabled(bState);
	SetActorHiddenInGame(!bState);
}

void AAcoBoss::SetupFlyingTowardsColumnLocation()
{
	if (!FlyTrailVfx) AssetsManager->AttachVfx(EVfx::AcoFlyTrail, nullptr, &FlyTrailVfx, GetMesh(), FName("RootSocket"));
	else FlyTrailVfx->Activate();
	bIsFlyingTowardsHijackTarget = false;
	ChangeActorRenderState(true);
	AcoAnimInstance->SetIsFlying(true);
	(void)DelegateManager->RequestTriggerCameraShake.ExecuteIfBound(ECameraShake::AcoDashInitiationCameraShake);
	GetWorldTimerManager().SetTimer(FlyingTowardsTargetHandle, this, &AAcoBoss::ProcessFlyingTowardsTarget, 0.025f, true);
}

void AAcoBoss::StandUp() const { AcoAnimInstance->PlayMontage(EEnemyAnimState::StandUp); }

void AAcoBoss::RiseUpwards()
{
	SetActorLocation(GetActorLocation() + FVector::UpVector * RisingUpwardsSpeed);
	UnitsRose += RisingUpwardsSpeed;
	if (UnitsRose >= UnitsToRise)
	{
		UE_LOG(LogTemp, Error, TEXT("REACHED SKY"));
		GetWorldTimerManager().ClearTimer(RiseUpwardsHandle);
	}
}
