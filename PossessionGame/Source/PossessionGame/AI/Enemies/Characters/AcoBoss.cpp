#include "AcoBoss.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Animation Instances/AcoBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/UI/MainWidgets/WaveManagerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

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
	LookAtLocation = GetActorLocation();
	Super::BeginPlay();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AcoAnimInstance = Cast<UAcoBossAnimationInstance>(GetAnimInstance());
	AcoAnimInstance->PlayMontage(EEnemyAnimState::Kneel);
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	(void)DelegateManager->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::Boss);
	DelegateManager->OnAcoEnemySpawned.BindUObject(this, &AAcoBoss::SetupFlyingTowardsHijackTarget);
	DelegateManager->OnFinalBossModeInitiated.BindUObject(this, &AAcoBoss::StandUp);
	DelegateManager->OnAcoEnemyDeath.BindUObject(this, &AAcoBoss::SetupFlyingTowardsColumnLocation);
	//DelegateManager->OnAcoPlatformRiseComplete.BindUObject(this, &AAcoBoss::InitiateSecondPhase);
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
	if (!HijackTargetEnemy) return LookAtLocation;
	return bIsFlyingTowardsHijackTarget ? HijackTargetEnemy->GetActorLocation() : LookAtLocation;
}

void AAcoBoss::SpawnFirstPhaseEnemy()
{
	if (CharactersToHijack.IsEmpty())
	{
		FirstPhaseFinished();
		return;
	}
	const int32 Index = FMath::RandRange(0, CharactersToHijack.Num() - 1);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->RequestSpawnEnemy(CharactersToHijack[Index], EGameMode::FinalBossEnemySpawn, nullptr, nullptr);
	CharactersToHijack.RemoveAt(Index);
}

void AAcoBoss::InitiateFight()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetWorldTimerManager().SetTimer(AbilityCycleHandle, this, &AAcoBoss::ProcessAbilityCycle, FrameRate, true);
}

void AAcoBoss::SetupFlyingTowardsHijackTarget(const TObjectPtr<AEnemy>& InHijackTarget)
{
	AcoAnimInstance->StopMontage(EEnemyAnimState::Kneel);
	if (!FlyTrailVfx) AssetsManager->AttachVfx(EVfx::AcoFlyTrail, nullptr, &FlyTrailVfx, GetMesh(), FName("RootSocket"));
	else FlyTrailVfx->Activate();
	bIsFlyingTowardsHijackTarget = true;
	HijackTargetEnemy = InHijackTarget;
	AcoAnimInstance->SetFlyingState(true);
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
	AcoAnimInstance->SetFlyingState(false);
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
	AcoAnimInstance->SetFlyingState(true);
	(void)DelegateManager->RequestTriggerCameraShake.ExecuteIfBound(ECameraShake::AcoDashInitiationCameraShake);
	GetWorldTimerManager().SetTimer(FlyingTowardsTargetHandle, this, &AAcoBoss::ProcessFlyingTowardsTarget, 0.025f, true);
}

void AAcoBoss::StandUp() const { AcoAnimInstance->PlayMontage(EEnemyAnimState::StandUp); }

void AAcoBoss::FirstPhaseFinished()
{
	(void)DelegateManager->OnAcoFirstPhaseFinish.ExecuteIfBound();
	AcoAnimInstance->PlayMontage(EEnemyAnimState::SlamPrepare);
	SetupSecondPhaseStartCountdown();
}

void AAcoBoss::InitiateSecondPhase()
{
	ChangeActorRenderState(true);
	AcoAnimInstance->PlayMontage(EEnemyAnimState::SlamFromSky);
	FTimerHandle SlamHandle;
	GetWorldTimerManager().SetTimer(SlamHandle, this, &AAcoBoss::TriggerSlamImpact, 0.3f);
}

void AAcoBoss::TriggerSlamImpact()
{
	(void)DelegateManager->RequestTriggerCameraShake.ExecuteIfBound(ECameraShake::AcoSlamCameraShake);
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, GetActorLocation(), FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(StompRadiusInMeters * 100.f));
	for (const FOverlapResult& Result : Overlaps)
	{
		TObjectPtr<AActor> HitActor = Result.GetActor();
		if (!HitActor || !HitActor->ActorHasTag("Player")) continue;
		UGameplayStatics::ApplyDamage(HitActor, StompDamage, nullptr, this, UDamageType::StaticClass());
		return;
	}
}

void AAcoBoss::SetupSecondPhaseStartCountdown()
{
	UpdateSecondPhaseCountdown();
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AAcoBoss::UpdateSecondPhaseCountdown, 1.f, true);
}

void AAcoBoss::UpdateSecondPhaseCountdown()
{
	const TObjectPtr<UWaveManagerHUD> WaveManagerUI = GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UWaveManagerHUD>(EWidget::WaveManagerHUD);
	WaveManagerUI->UpdateCountdown(--CountdownSecs);
	WaveManagerUI->PlayCountdownAnim();
	if (CountdownSecs <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		InitiateSecondPhase();
	}
}

void AAcoBoss::ProcessAbilityCycle()
{
	if (bIsUltraInstinctActive) return;
	UltraInstinctTimer += FrameRate;
	if (UltraInstinctTimer >= UltraInstinctCooldown)
	{
		UE_LOG(LogTemp, Error, TEXT("ULTRA INSTINCT ENABLED"));
		bIsUltraInstinctActive = true;
		UltraInstinctTimer = 0.f;
		CurrentUltraInstinctPose = FMath::RandRange(0, 3);
		LookAtLocation = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetTargetLocation();
		AcoAnimInstance->SetUltraInstinctPose(CurrentUltraInstinctPose);
		AcoAnimInstance->SetUltraInstinctState(true);
	}
}

float AAcoBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsUltraInstinctActive)
	{
		const int32 Offset = FMath::RandRange(1, 3);
		CurrentUltraInstinctPose = (CurrentUltraInstinctPose + Offset) % 4;
		LookAtLocation = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetTargetLocation();
		AcoAnimInstance->SetUltraInstinctPose(CurrentUltraInstinctPose);
		UltraInstinctTeleport();
		return 0.f;
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AAcoBoss::UltraInstinctTeleport()
{
	FVector CurrentLocation = GetActorLocation();
	int32 CurrentAttempt = 0;
	bool bFoundValidSpot = false;
	do 
	{
		const float OffsetX = FMath::FRandRange(MinTeleportDistance, MaxTeleportDistance) * (FMath::RandBool() ? 1.f : -1.f);
		const float OffsetY = FMath::FRandRange(MinTeleportDistance, MaxTeleportDistance) * (FMath::RandBool() ? 1.f : -1.f);
		FVector PotentialTarget = FVector(CurrentLocation.X + OffsetX, CurrentLocation.Y + OffsetY, CurrentLocation.Z);
		if (FVector::Dist(PotentialTarget, FinalBossArenaCenter) <= ArenaRadius)
		{
			CurrentLocation = PotentialTarget;
			bFoundValidSpot = true;
		}
		CurrentAttempt++;
	} while (!bFoundValidSpot && CurrentAttempt < MaxAttempts);
	if (bFoundValidSpot) SetActorLocation(CurrentLocation);
}
