#include "MinionBoss.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/TriggerSphere.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void AMinionBoss::ActivateStompTrigger()
{
	bStompIsReady = true;
	if (!bIsAbilityInAction) ToggleStompTrigger(true);
}

void AMinionBoss::TriggerStompCameraShake() const { (void)DelegateManager->OnMinionBossStomp.ExecuteIfBound(StompCameraShake); }

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
	SetIsActionInMotion(false);
	SetIsAbilityInAction(false);
	if (bStompIsReady) ToggleStompTrigger(true);
}

AMinionBoss::AMinionBoss()
{
	if (AI_HpBarComponent)
	{
		AI_HpBarComponent->DestroyComponent();
		AI_HpBarComponent = nullptr;
	}
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
}

TObjectPtr<AWeapon>& AMinionBoss::GetWeapon() { return Weapons[0]; }

void AMinionBoss::AI_SetupWeapon()
{
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

void AMinionBoss::BeginPlay()
{
	CharacterName = ECharacterType::MinionBoss;
	Super::BeginPlay();
	LaserCollision->OnComponentBeginOverlap.AddDynamic(this, &AMinionBoss::OnLaserCollisionBeginOverlap);
	LaserCollision->OnComponentEndOverlap.AddDynamic(this, &AMinionBoss::OnLaserCollisionEndOverlap);
	(void)DelegateManager->OnBossSpawn.ExecuteIfBound();
	SetupStompTrigger();
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

void AMinionBoss::SetupStompTrigger()
{
	StompTriggerSphere = GetWorld()->SpawnActor<ATriggerSphere>(ATriggerSphere::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	StompTriggerSphere->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	StompTriggerSphere->GetCollisionComponent()->SetCollisionProfileName("TriggerPlayer");
	StompTriggerSphere->GetCollisionComponent()->CanCharacterStepUpOn = ECB_No;
	StompTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Cast<USphereComponent>(StompTriggerSphere->GetCollisionComponent())->SetSphereRadius(StompTriggerRadiusInMeters * 100.f);
	StompTriggerSphere->OnActorBeginOverlap.AddDynamic(this, &AMinionBoss::OnPlayerEnterStompTrigger);
}

void AMinionBoss::DestroyStompTriggerSphere() const
{
	if (StompTriggerSphere)
	{
		StompTriggerSphere->OnActorBeginOverlap.RemoveAll(this);
		StompTriggerSphere->Destroy();
	}
}

void AMinionBoss::InitiateDeath()
{
	if (MI_Defeated) MI_MeshBaseMaterial = UMaterialInstanceDynamic::Create(MI_Defeated, this);
	(void)StopLaser.ExecuteIfBound();
	DestroyStompTriggerSphere();
	GetMesh()->SetMaterial(0, MI_MeshBaseMaterial);
	SpawnCoins();
	Super::InitiateDeath();
	DelegateManager->OnBossDeath.Broadcast();
}

void AMinionBoss::ToggleStompTrigger(const bool bState) const
{
	if (StompTriggerSphere) StompTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AMinionBoss::SetEmpEffect(const float InEffectDuration)
{
	switch (GetAI()->GetCurrentStateTreeEvent())
	{
		case EStateTreeEvent::MinionBossStomp: AI_StopAbility(0); break;
		case EStateTreeEvent::MinionBossLaserEye: AI_StopAbility(1); break;
		default: ;
	}
	Super::SetEmpEffect(InEffectDuration);
}

void AMinionBoss::EmpEffectEnded()
{
	Super::EmpEffectEnded();
	if (!StompTriggerSphere) return;
	StompTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StompTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMinionBoss::ReceiveDamage(const float InDamage, const EDamageReason InDamageReason)
{
	Super::ReceiveDamage(InDamage, InDamageReason);
	(void)DelegateManager->OnBossDamaged.ExecuteIfBound(CurrentHp, MaxHp);
}

void AMinionBoss::CheckDistanceOfLaserEyeTrigger()
{
	if (!AI || bIsAbilityInAction || bAffectedByEmp) return;
	const TObjectPtr<AActor> Target = AI->GetStateTreeData().GetTarget();
	if (!Target) return;
	if (FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) >= FMath::Square(1000.f))
	{
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossLaserEye);
		GetWorld()->GetTimerManager().ClearTimer(CheckLaserEyeDistanceHandle);
	}
}

void AMinionBoss::DealLaserDamageToPlayer() const { if (Player) Player->ReceiveDamage(2.f, EDamageReason::MinionBossDamage); }

void AMinionBoss::SpawnCoins() const
{
	const FVector Location = GetSocketLocation("StompSocket");
	const TObjectPtr<UPoolingManager> PoolingManager = GetWorld()->GetSubsystem<UPoolingManager>();
	switch (FMath::RandRange(1, 3))
	{
		case 1: PoolingManager->SpawnObjectFromPool(EPoolObject::Coin, Location + FVector(-35.f, 0.f, 120.f))->Init(); break;
		case 2:
			PoolingManager->SpawnObjectFromPool(EPoolObject::Coin, Location + FVector(15.f, 0.f, 120.f))->Init();
			PoolingManager->SpawnObjectFromPool(EPoolObject::Coin, Location - FVector(15.f, 0.f, 120.f))->Init();
			break;
		case 3:
			PoolingManager->SpawnObjectFromPool(EPoolObject::Coin, Location + FVector(-35.f, 30.f, 120.f))->Init();
			PoolingManager->SpawnObjectFromPool(EPoolObject::Coin, Location + FVector(-85.f, -30.f, 120.f))->Init();
			PoolingManager->SpawnObjectFromPool(EPoolObject::Coin, Location + FVector(15.f, -30.f, 120.f))->Init();
		default: ;
	}
}

void AMinionBoss::OnPlayerEnterStompTrigger(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!AI || bAffectedByEmp) return;
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossStomp);
	ToggleStompTrigger(false);
	bStompIsReady = false;
}

void AMinionBoss::OnLaserCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor); Enemy->GetIsPossessed())
	{
		Player = Enemy;
		GetWorld()->GetTimerManager().SetTimer(DealLaserDamageToPlayerHandle, this, &AMinionBoss::DealLaserDamageToPlayer, 0.05f, true);
	}
}

void AMinionBoss::OnLaserCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor); Enemy->GetIsPossessed())
	{
		Player = nullptr;
		GetWorld()->GetTimerManager().ClearTimer(DealLaserDamageToPlayerHandle);
	}
}
