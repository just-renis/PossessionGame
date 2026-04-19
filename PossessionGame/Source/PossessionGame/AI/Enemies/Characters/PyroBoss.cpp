#include "PyroBoss.h"
#include "NiagaraComponent.h"
#include "Algo/RandomShuffle.h"
#include "Components/AudioComponent.h"
#include "Engine/OverlapResult.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void APyroBoss::MarkStompAbilityReady()
{
	bFirstStompSucceed = false;
	GetWorldTimerManager().SetTimer(ActivateStompHandle, this, &APyroBoss::ActivateStomp, 0.25f, true);
}

void APyroBoss::MarkBallPhaseAbilityReady()
{
	GetWorldTimerManager().SetTimer(ActivateBallPhaseHandle, this, &APyroBoss::ActivateBallPhase, 0.25f, true);
}

void APyroBoss::InitiateBallPhase()
{
	bIsBallPhaseActive = true;
	FTimerHandle ActivateBallPhaseAttackHandle;
	GetWorldTimerManager().SetTimer(ActivateBallPhaseAttackHandle, this, &APyroBoss::ActivateBallPhaseAttack, GetAnimInstance()->GetMontageLength(EEnemyAnimState::ActivatePyroBall));
}

void APyroBoss::FinishedBallPhase() { bIsBallPhaseActive = false; }

void APyroBoss::StompHitTheGround(const bool bFirstStomp)
{
	(void)DelegateManager->RequestTriggerCameraShake.ExecuteIfBound(bFirstStomp ? ECameraShake::PyroBossStompCameraShake1 : ECameraShake::PyroBossStompCameraShake2);
	//Proprietor->PlaySound(ESound::MinionBossStomp, true);
	//Proprietor->GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::MinionBossStompCrack, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, Proprietor->GetSocketLocation("StompSocket"));
	const FVector StompLocation = GetSocketLocation("StompSocket");
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, StompLocation, FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(GetStompRadius() * 100.f));
	TSet<TObjectPtr<AActor>> UniqueActors;
	for (const FOverlapResult& Result : Overlaps)
	{
		TObjectPtr<AActor> HitActor = Result.GetActor();
		if (!HitActor || UniqueActors.Contains(HitActor)) continue;
		UniqueActors.Add(HitActor);
		if (HitActor->ActorHasTag("Player"))
		{
			Cast<AEnemy>(HitActor)->P_ReceiveStunEffect(bFirstStompSucceed ? 0.f : GetStunDuration(), GetStompDamage(), FVector::ZeroVector);
			bFirstStompSucceed = true;
			return;
		}
	}
}

APyroBoss::APyroBoss()
{
	AI_DestroyBossUnnecessaryComponents();
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>("SpawnSfx");
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(GetMesh());
	DeathSfx = CreateDefaultSubobject<UAudioComponent>("DeathSfx");
	DeathSfx->SetAutoActivate(false);
	DeathSfx->SetupAttachment(GetMesh());
	TakeDamageSfx = CreateDefaultSubobject<UAudioComponent>("TakeDamageSfx");
	TakeDamageSfx->SetAutoActivate(false);
	TakeDamageSfx->SetupAttachment(GetMesh());
	StompSfx = CreateDefaultSubobject<UAudioComponent>("StompSfx");
	StompSfx->SetAutoActivate(false);
	StompSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::PyroBossSpawn, SpawnSfx);
	Sounds.Add(ESound::PyroBossDeath, DeathSfx);
	Sounds.Add(ESound::PyroBossTakeDamage, TakeDamageSfx);
	Sounds.Add(ESound::PyroBossStomp, StompSfx);
}

void APyroBoss::AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	if (bHitByOtherAI) return;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), InDamage, GetActorLocation(), GetActorForwardVector());
	ReceiveDamage(InDamage, EDamageReason::BreacherBashDamage, nullptr, nullptr);
}

void APyroBoss::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::PyroBossWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void APyroBoss::AI_PlaySpawnSound() { PlaySound(ESound::PyroBossSpawn, true); }

void APyroBoss::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::PyroBossDeath);
}

void APyroBoss::BeginPlay()
{
	CharacterName = ECharacterType::PyroBoss;
	bIsBoss = true;
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(FloorTriggerHandle, this, &APyroBoss::ActivateFloorLava, FloorTriggerTime, true);
	(void)DelegateManager->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::Boss, 1);
}

void APyroBoss::InitiateDeath()
{
	Super::InitiateDeath();
	DelegateManager->OnBossDeath.Broadcast();
}

void APyroBoss::ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent)
{
	Super::ReceiveDamage(InDamage, InDamageReason, PointDamageEvent, RadialDamageEvent);
	if (FMath::RandRange(1, 10) >= 9) PlaySound(ESound::PyroBossTakeDamage);
	(void)DelegateManager->UpdateGlobalHpBar.ExecuteIfBound(CurrentHp, MaxHp);
}

void APyroBoss::ApplyEmpEffect(const float InEffectDuration) { Super::ApplyEmpEffect(InEffectDuration * 0.5f); }

void APyroBoss::ActivateFloorLava() const
{
	const float HpPercentageLeft = CurrentHp / MaxHp;
	const int32 NumIndicesToUse = HpPercentageLeft > SecondPhaseHpPercentage ? 1.f : HpPercentageLeft > ThirdPhaseHpPercentage ? 2.f : 3.f;
	TArray Indices = { 0, 1, 2, 3, 4, 5, 6, 7 };
	Algo::RandomShuffle(Indices);
	Indices.SetNum(NumIndicesToUse);
	DelegateManager->TriggerFireFloors.Broadcast(Indices);
}

void APyroBoss::ActivateStomp()
{
	if (!AI || IsActionsDisabled() || bIsActionInMotion || !AI->GetStateTreeData().GetStateTreeRef()) return;
	PlaySound(ESound::PyroBossStomp);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, GetActorLocation(), FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(StompTriggerRadiusInMeters * 100.f), Params);
#if WITH_EDITOR
	if (bEnableDebug) DrawDebugSphere(GetWorld(), GetActorLocation(), StompTriggerRadiusInMeters * 100.f, 32, FColor::Green, false, 3.f);
#endif
	for (const FOverlapResult& Overlap : Overlaps)
	{
		TObjectPtr<AActor> Actor = Overlap.GetActor();
		if (!Actor || !Actor->ActorHasTag("Player")) continue;
		SetIsActionInMotion(true);
		GetWorldTimerManager().ClearTimer(ActivateStompHandle);
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::PyroBossStomp);
		return;
	}
}

void APyroBoss::ActivateBallPhase()
{
	if (!AI || IsActionsDisabled() || bIsActionInMotion || !AI->GetStateTreeData().GetStateTreeRef()) return;
	GetWorldTimerManager().ClearTimer(ActivateBallPhaseHandle);
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::PyroBossBallPhase);
}

void APyroBoss::ActivateBallPhaseAttack()
{
	if (!IsAsyncAssetStillAllowedToBeUsed()) return;
	GetWorldTimerManager().SetTimer(BallPhaseAttackHandle, this, &APyroBoss::TriggerBallPhaseAttack, ShotFrequency, true);
	if (!BallPhaseVfx) AssetsManager->AttachVfx(EVfx::PyroBossBallPhase, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed() && bIsBallPhaseActive; }, &BallPhaseVfx, GetMesh(), FName("BallPhaseSocket"));
	else BallPhaseVfx->Activate();
}

void APyroBoss::TriggerBallPhaseAttack()
{
	if (!bIsBallPhaseActive)
	{
		GetWorldTimerManager().ClearTimer(BallPhaseAttackHandle);
		return;
	}
	const FRotator Rotation = FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f);
	ProjectileData.SetProprietor(this);
	ProjectileData.GetMultiProjectileDataPtr(1)->Data[0].GravityScale = FMath::FRandRange(MinGravityScale, MaxGravityScale);
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::PyroBossProjectile, GetSocketLocation(FName("BallPhaseSocket")), Rotation)->Init(ProjectileData, Rotation);
}

void APyroBoss::ClearTimers()
{
	Super::ClearTimers();
	GetWorldTimerManager().ClearTimer(FloorTriggerHandle);
	GetWorldTimerManager().ClearTimer(ActivateStompHandle);
	GetWorldTimerManager().ClearTimer(ActivateBallPhaseHandle);
	GetWorldTimerManager().ClearTimer(BallPhaseAttackHandle);
}
