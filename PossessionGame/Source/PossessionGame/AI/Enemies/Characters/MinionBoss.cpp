#include "MinionBoss.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void AMinionBoss::ActivateStompTrigger()
{
	bStompIsReady = true;
	if (!bIsAbilityInAction) ToggleStompTrigger(true);
}

void AMinionBoss::TriggerStompCameraShake() const { (void)DelegateManager->RequestTriggerCameraShake.ExecuteIfBound(ECameraShake::MinionBossStomp); }

void AMinionBoss::TriggerLaserCameraShake() const { (void)DelegateManager->RequestTriggerCameraShake.ExecuteIfBound(ECameraShake::MinionBossLaser); }

void AMinionBoss::EnableLaserEyeBall()
{
	ToggleStompTrigger(false);
	if (!LaserEyeBallVfx) AssetsManager->AttachVfx(EVfx::MinionBossLaserBall, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &LaserEyeBallVfx, GetMesh(), "LaserSocket");
	else LaserEyeBallVfx->Activate();
}

void AMinionBoss::EnableLaser()
{
	if (!LaserVfx) AssetsManager->AttachVfx(EVfx::MinionBossLaser, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &LaserVfx, GetMesh(), "LaserSocket");
	else LaserVfx->Activate();
}

void AMinionBoss::EnableDistanceCheckForLaserEye() { GetWorld()->GetTimerManager().SetTimer(CheckLaserEyeDistanceHandle, this, &AMinionBoss::CheckDistanceOfLaserEyeTrigger, 0.2f, true); }

void AMinionBoss::OnLaserFinish()
{
	if (!AI->GetStateTreeData().GetStateTreeRef()) return;
	SetIsActionInMotion(false);
	SetIsAbilityInAction(false);
	if (bIsMoveDirectlyToPlayerTriggered) AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossMoveDirectlyToPlayer);
	if (bStompIsReady) ToggleStompTrigger(true);
}

void AMinionBoss::OnStompFinish()
{
	if (!AI->GetStateTreeData().GetStateTreeRef()) return;
	SetIsActionInMotion(false);
	SetIsAbilityInAction(false);
	if (bIsMoveDirectlyToPlayerTriggered) AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossMoveDirectlyToPlayer);
}

void AMinionBoss::ResetMoveDirectlyToPlayer()
{
	bIsMoveDirectlyToPlayerTriggered = false;
	SetMovementSpeedMultiplier(1.f);
	GetWorld()->GetTimerManager().SetTimer(MoveDirectlyToPlayerHandle, this, &AMinionBoss::SetMoveDirectlyToPlayer, MoveDirectlyToPlayerInterval, false);
}

AMinionBoss::AMinionBoss()
{
	if (AI_HpBarComponent)
	{
		AI_HpBarComponent->DestroyComponent();
		AI_HpBarComponent = nullptr;
	}
	StompTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("StompTrigger"));
	StompTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	StompTrigger->SetCollisionProfileName("TriggerPlayer");
	StompTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StompTrigger->CanCharacterStepUpOn = ECB_No;
	StompTrigger->SetSphereRadius(StompTriggerRadiusInMeters * 100.f);
	StompTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMinionBoss::OnPlayerEnterStompTrigger);
	LaserCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("laserCollision"));
	LaserCollision->SetupAttachment(GetMesh());
	LaserCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sounds.Add(ESound::MinionBossStomp, CreateDefaultSubobject<UAudioComponent>(TEXT("stompSfx")));
	Sounds[ESound::MinionBossStomp]->SetAutoActivate(false);
	Sounds[ESound::MinionBossStomp]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::MinionBossLaserEyeCharge, CreateDefaultSubobject<UAudioComponent>(TEXT("laserEyeChargeSfx")));
	Sounds[ESound::MinionBossLaserEyeCharge]->SetAutoActivate(false);
	Sounds[ESound::MinionBossLaserEyeCharge]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::MinionBossLaserEyeHold, CreateDefaultSubobject<UAudioComponent>(TEXT("laserEyeHoldSfx")));
	Sounds[ESound::MinionBossLaserEyeHold]->SetAutoActivate(false);
	Sounds[ESound::MinionBossLaserEyeHold]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::MinionBossDeathExplosion, CreateDefaultSubobject<UAudioComponent>(TEXT("deathExplosionSfx")));
	Sounds[ESound::MinionBossDeathExplosion]->SetAutoActivate(false);
	Sounds[ESound::MinionBossDeathExplosion]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::MinionBossElectrificationDeathPassive, CreateDefaultSubobject<UAudioComponent>(TEXT("electrificationDeathPassiveSfx")));
	Sounds[ESound::MinionBossElectrificationDeathPassive]->SetAutoActivate(false);
	Sounds[ESound::MinionBossElectrificationDeathPassive]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::MinionBossSpawn, CreateDefaultSubobject<UAudioComponent>(TEXT("spawnSound")));
	Sounds[ESound::MinionBossSpawn]->SetAutoActivate(false);
	Sounds[ESound::MinionBossSpawn]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::MinionBossDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("deathSound")));
	Sounds[ESound::MinionBossDeath]->SetAutoActivate(false);
	Sounds[ESound::MinionBossDeath]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::MinionBossTakeDamage, CreateDefaultSubobject<UAudioComponent>(TEXT("takeDamageSound")));
	Sounds[ESound::MinionBossTakeDamage]->SetAutoActivate(false);
	Sounds[ESound::MinionBossTakeDamage]->SetupAttachment(GetMesh());
}

TObjectPtr<AWeapon>& AMinionBoss::GetWeapon() { return Weapons[0]; }

void AMinionBoss::AI_SetupWeapon()
{
	if (!AI) return;
	for (int32 i = 0; i < 4; i++)
	{
		Weapons.Add(GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::MinionBossHandWeapon, GetMesh(), FName("AIWeaponSocket" + FString::FromInt(i + 1))));
		Weapons[i]->SetProprietor(this);
		Weapons[i]->PostProprietorSetup();
	}
	AI->SetShootCooldown(Weapons[0]->GetShootCooldown());
}

void AMinionBoss::AI_ShootLogic()
{
	if (!bIsActionInMotion && bCanShoot) AI_Shoot();
}

void AMinionBoss::AI_Shoot()
{
	Weapons[CurrentWeaponIndex++]->AI_Shoot();
	if (CurrentWeaponIndex >= 4) CurrentWeaponIndex = 0;
}

void AMinionBoss::AI_SetCanShoot(const bool bState)
{
	bCanShoot = bState;
	if (!bCanShoot) AI_SetShootPause();
}

void AMinionBoss::AI_CancelShooting()
{
	CurrentWeaponIndex = 0;
	if (!AnimInstance) return;
	AnimInstance->StopMontage(EEnemyAnimState::Shoot);
}

void AMinionBoss::AI_InitiateShooting() { AI_PlayShootMontage(); }

void AMinionBoss::AI_PlayDeathExplosion()
{
	PlaySound(ESound::MinionBossDeathExplosion);
	PlaySound(ESound::MinionBossElectrificationDeathPassive);
	AssetsManager->AttachVfx(EVfx::MinionBossDeathExplosion, nullptr, nullptr, GetMesh(), "DeathSocket");
}

void AMinionBoss::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::MinionBossDeathElectrification, nullptr, nullptr, GetMesh(), "DeathSocket");
}

void AMinionBoss::AI_PlaySpawnSound() { PlaySound(ESound::MinionBossSpawn, true); }

void AMinionBoss::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::MinionBossDeath);
}

void AMinionBoss::AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	if (bHitByOtherAI) return;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), InDamage, GetActorLocation(), GetActorForwardVector());
	ReceiveDamage(InDamage, EDamageReason::BreacherBashDamage);
}

void AMinionBoss::BeginPlay()
{
	CharacterName = ECharacterType::MinionBoss;
	Super::BeginPlay();
	LaserCollision->OnComponentBeginOverlap.AddDynamic(this, &AMinionBoss::OnLaserCollisionBeginOverlap);
	LaserCollision->OnComponentEndOverlap.AddDynamic(this, &AMinionBoss::OnLaserCollisionEndOverlap);
	(void)DelegateManager->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::MinionBoss);
	GetWorld()->GetTimerManager().SetTimer(MoveDirectlyToPlayerHandle, this, &AMinionBoss::SetMoveDirectlyToPlayer, MoveDirectlyToPlayerInterval, false);
}

void AMinionBoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(CheckLaserEyeDistanceHandle);
	GetWorld()->GetTimerManager().ClearTimer(DealLaserDamageToPlayerHandle);
	Super::EndPlay(EndPlayReason);
}

void AMinionBoss::HandleDeactivation()
{
	for (int32 i = 0; i < Weapons.Num(); i++) Weapons[i]->DeactivateWeapon();
	Super::HandleDeactivation();
}

void AMinionBoss::DestroyThis()
{
	for (int32 i = 0; i < Weapons.Num(); i++) Weapons[i]->DestroyThis();
	Super::DestroyThis();
}

void AMinionBoss::DestroyStompTriggerSphere() const
{
	if (StompTrigger)
	{
		StompTrigger->OnComponentBeginOverlap.RemoveAll(this);
		StompTrigger->DestroyComponent();
	}
}

void AMinionBoss::InitiateDeath()
{
	GetWorld()->GetTimerManager().ClearTimer(MoveDirectlyToPlayerHandle);
	if (MI_Defeated) MI_MeshBaseMaterial = UMaterialInstanceDynamic::Create(MI_Defeated, this);
	(void)StopLaser.ExecuteIfBound();
	DestroyStompTriggerSphere();
	GetMesh()->SetMaterial(0, MI_MeshBaseMaterial);
	Super::InitiateDeath();
	DelegateManager->OnBossDeath.Broadcast();
}

void AMinionBoss::ToggleStompTrigger(const bool bState) const
{
	if (StompTrigger) StompTrigger->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AMinionBoss::EmpEffectEnded()
{
	Super::EmpEffectEnded();
	if (!StompTrigger || StompTrigger->GetCollisionEnabled() != ECollisionEnabled::QueryOnly) return;
	StompTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StompTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMinionBoss::ReceiveDamage(const float InDamage, const EDamageReason InDamageReason)
{
	Super::ReceiveDamage(InDamage, InDamageReason);
	if (FMath::RandRange(1, 10) >= 9) PlaySound(ESound::MinionBossTakeDamage);
	(void)DelegateManager->UpdateGlobalHpBar.ExecuteIfBound(CurrentHp, MaxHp);
}

void AMinionBoss::CheckDistanceOfLaserEyeTrigger()
{
	if (!AI || bIsAbilityInAction || IsActionsDisabled() || !AI->GetStateTreeData().GetStateTreeRef()) return;
	const TObjectPtr<AActor> Target = AI->GetStateTreeData().GetTarget();
	if (!Target) return;
	if (FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) >= FMath::Square(1000.f))
	{
		if (bIsMoveDirectlyToPlayerTriggered) ResetMoveDirectlyToPlayer();
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossLaserEye);
		GetWorld()->GetTimerManager().ClearTimer(CheckLaserEyeDistanceHandle);
	}
}

void AMinionBoss::DealLaserDamageToPlayer() const { if (Player) Player->ReceiveDamage(2.5f, EDamageReason::MinionBossDamage); }

void AMinionBoss::SetMoveDirectlyToPlayer()
{
	if (!AI->GetStateTreeData().GetStateTreeRef()) return;
	bIsMoveDirectlyToPlayerTriggered = true;
	if (!bIsAbilityInAction && !bIsActionInMotion) AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossMoveDirectlyToPlayer);
}

void AMinionBoss::OnPlayerEnterStompTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!AI || IsActionsDisabled() || OtherActor->ActorHasTag("AliveEnemy") || bIsActionInMotion || !AI->GetStateTreeData().GetStateTreeRef()) return;
	if (bIsMoveDirectlyToPlayerTriggered) ResetMoveDirectlyToPlayer();
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossStomp);
	ToggleStompTrigger(false);
	bStompIsReady = false;
}

void AMinionBoss::OnLaserCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor); Enemy->GetIsHijacked())
	{
		Player = Enemy;
		GetWorld()->GetTimerManager().SetTimer(DealLaserDamageToPlayerHandle, this, &AMinionBoss::DealLaserDamageToPlayer, 0.05f, true);
	}
}

void AMinionBoss::OnLaserCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor); Enemy->GetIsHijacked())
	{
		Player = nullptr;
		GetWorld()->GetTimerManager().ClearTimer(DealLaserDamageToPlayerHandle);
	}
}
