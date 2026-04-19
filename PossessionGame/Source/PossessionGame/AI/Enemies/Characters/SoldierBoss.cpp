#include "SoldierBoss.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/AI/Others/BackpackAnimInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

void ASoldierBoss::MarkMissilesLaunchAbilityReady()
{
	GetWorldTimerManager().SetTimer(ActivateMissilesLaunchHandle, this, &ASoldierBoss::ActivateMissilesLaunch, 0.25f, true);
}

void ASoldierBoss::StartMissilesLaunchPhase(const float InDuration)
{
	GetAnimInstance()->PlayMontageWithHold(EEnemyAnimState::MissilesLaunch, InDuration, false);
	GetWorldTimerManager().SetTimer(MissileLaunchHandle, this, &ASoldierBoss::LaunchMissile, MissileLaunchInterval, true);
	if (!BackpackAnimInstance) return;
	BackpackAnimInstance->SetIsOpen(true);
}

void ASoldierBoss::FinishedMissilesLaunch()
{
	if (MissileLaunchVfx) MissileLaunchVfx->DeactivateImmediate();
	GetWorldTimerManager().ClearTimer(MissileLaunchHandle);
	if (!BackpackAnimInstance) return;
	BackpackAnimInstance->SetIsOpen(false);
}

ASoldierBoss::ASoldierBoss()
{
	AI_DestroyBossUnnecessaryComponents();
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSfx"));
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(GetMesh());
	DeathSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathSfx"));
	DeathSfx->SetAutoActivate(false);
	DeathSfx->SetupAttachment(GetMesh());
	TakeDamageSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("TakeDamageSfx"));
	TakeDamageSfx->SetAutoActivate(false);
	TakeDamageSfx->SetupAttachment(GetMesh());
	MissileLaunchSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("MissileLaunchSfx"));
	MissileLaunchSfx->SetAutoActivate(false);
	MissileLaunchSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::SoldierBossSpawn, SpawnSfx);
	Sounds.Add(ESound::SoldierBossDeath, DeathSfx);
	Sounds.Add(ESound::SoldierBossTakeDamage, TakeDamageSfx);
	Sounds.Add(ESound::SoldierBossMissileLaunch, MissileLaunchSfx);
	Backpack = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Backpack"));
}

void ASoldierBoss::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::SoldierBossWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ASoldierBoss::AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	if (bHitByOtherAI) return;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), InDamage, GetActorLocation(), GetActorForwardVector());
	ReceiveDamage(InDamage, EDamageReason::BreacherBashDamage, nullptr, nullptr);
}

void ASoldierBoss::AI_PlaySpawnSound() { PlaySound(ESound::SoldierBossSpawn, true); }

void ASoldierBoss::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::SoldierBossDeath);
}

void ASoldierBoss::BeginPlay()
{
	CharacterName = ECharacterType::SoldierBoss;
	bIsBoss = true;
	Super::BeginPlay();
	(void)DelegateManager->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::Boss, 1);
	if (!Backpack) return;
	Backpack->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("BackpackSocket"));
	BackpackAnimInstance = Cast<UBackpackAnimInstance>(Backpack->GetAnimInstance());
}

void ASoldierBoss::ApplyEmpEffect(const float InEffectDuration) { Super::ApplyEmpEffect(InEffectDuration * 0.5f); }

void ASoldierBoss::ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent)
{
	Super::ReceiveDamage(InDamage, InDamageReason, PointDamageEvent, RadialDamageEvent);
	if (FMath::RandRange(1, 10) >= 9) PlaySound(ESound::SoldierBossTakeDamage);
	(void)DelegateManager->UpdateGlobalHpBar.ExecuteIfBound(CurrentHp, MaxHp);
}

void ASoldierBoss::InitiateDeath()
{
	Super::InitiateDeath();
	DelegateManager->OnBossDeath.Broadcast();
}

void ASoldierBoss::ActivateMissilesLaunch()
{
	if (!AI || IsActionsDisabled() || bIsActionInMotion || !AI->GetStateTreeData().GetStateTreeRef()) return;
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::SoldierBossMissilesLaunch);
	GetWorldTimerManager().ClearTimer(ActivateMissilesLaunchHandle);
}

void ASoldierBoss::ClearTimers()
{
	Super::ClearTimers();
	GetWorldTimerManager().ClearTimer(ActivateMissilesLaunchHandle);
	GetWorldTimerManager().ClearTimer(MissileLaunchHandle);
}

void ASoldierBoss::LaunchMissile()
{
	if (!MissileLaunchVfx) AssetsManager->AttachVfx(EVfx::SoldierBossMissileLaunched, nullptr, &MissileLaunchVfx, Backpack, FName("MissileSocket"));
	else MissileLaunchVfx->Activate(true);
	PlaySound(ESound::SoldierBossMissileLaunch);
	const TObjectPtr<AEnemy> Player = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetTarget();
	const FVector PlayerLocation = Player->GetSocketLocation(FName("RootSocket"));
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::SoldierBossMissile, PlayerLocation)->InitWithProprietor(this);
	if (!MissileImpactVfx) MissileImpactVfx = GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnAndReceiveVfxForcefully(EVfx::SoldierBossMissileImpact, PlayerLocation, FRotator::ZeroRotator);
	else
	{
		MissileImpactVfx->SetWorldLocation(PlayerLocation);
		MissileImpactVfx->Activate(true);
	}
}
