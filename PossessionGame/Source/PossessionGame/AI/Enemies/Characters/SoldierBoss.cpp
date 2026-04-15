#include "SoldierBoss.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

void ASoldierBoss::MarkMissilesLaunchAbilityReady()
{
	GetWorldTimerManager().SetTimer(ActivateMissilesLaunchHandle, this, &ASoldierBoss::ActivateMissilesLaunch, 0.25f, true);
}

void ASoldierBoss::PlayMissilesLaunchReadyAnims(const float InDuration)
{
	GetAnimInstance()->PlayMontageWithHold(EEnemyAnimState::MissilesLaunch, InDuration, false);
	if (Backpack) Backpack->PlayAnimation(BackpackOpenAnim, false);
}

void ASoldierBoss::FinishedMissilesLaunch() const
{
	if (Backpack) Backpack->PlayAnimation(BackpackCloseAnim, false);
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
	Sounds.Add(ESound::SoldierBossSpawn, SpawnSfx);
	Sounds.Add(ESound::SoldierBossDeath, DeathSfx);
	Sounds.Add(ESound::SoldierBossTakeDamage, TakeDamageSfx);
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
	if (Backpack) Backpack->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("BackpackSocket"));
	(void)DelegateManager->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::Boss, 1);
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
}

void ASoldierBoss::ClearTimers()
{
	Super::ClearTimers();
	GetWorldTimerManager().ClearTimer(ActivateMissilesLaunchHandle);
}
