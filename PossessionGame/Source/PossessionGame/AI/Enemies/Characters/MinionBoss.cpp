#include "MinionBoss.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/TriggerSphere.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraVariationData.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void AMinionBoss::ActivateStompTrigger()
{
	bStompIsReady = true;
	if (!bAbilityInAction) ToggleStompTrigger(true);
}

void AMinionBoss::TriggerStompCameraShake() const { (void)DelegateManager->OnMinionBossStomp.ExecuteIfBound(StompCameraShake); }

void AMinionBoss::EnableLaserEyeBall()
{
	if (bStompIsReady) ToggleStompTrigger(false);
	if (!LaserEyeBallVfx) AssetsManager->AttachVfx(EVfx::MinionBossLaserBall, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, FNiagaraVariationData(), &LaserEyeBallVfx, GetMesh(), "LaserSocket");
	else LaserEyeBallVfx->Activate();
}

void AMinionBoss::EnableLaser()
{
	if (!LaserVfx) AssetsManager->AttachVfx(EVfx::MinionBossLaser, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, FNiagaraVariationData(), &LaserVfx, GetMesh(), "LaserSocket");
	else LaserVfx->Activate();
}

void AMinionBoss::EnableDistanceCheckForLaserEye() { UKismetSystemLibrary::K2_SetTimer(this, "CheckDistanceOfLaserEyeTrigger", 0.2f, true); }

void AMinionBoss::OnLaserFinish()
{
	SetActionInMotion(false);
	SetAbilityInAction(false);
	if (bStompIsReady) ToggleStompTrigger(true);
}

AMinionBoss::AMinionBoss()
{
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
	AssetsManager->AttachVfx(EVfx::MinionBossDeathExplosion, nullptr, FNiagaraVariationData(), nullptr, GetMesh(), "DeathSocket");
}

void AMinionBoss::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::MinionBossDeathElectrification, nullptr, FNiagaraVariationData(), nullptr, GetMesh(), "DeathSocket");
}

TObjectPtr<AWeapon>& AMinionBoss::GetWeapon() { return Weapons[0]; }

void AMinionBoss::BeginPlay()
{
	CharacterName = ECharacterType::MinionBoss;
	Super::BeginPlay();
	LaserCollision->OnComponentBeginOverlap.AddDynamic(this, &AMinionBoss::OnLaserCollisionBeginOverlap);
	LaserCollision->OnComponentEndOverlap.AddDynamic(this, &AMinionBoss::OnLaserCollisionEndOverlap);
	(void)DelegateManager->OnBossSpawn.ExecuteIfBound();
	SetupStompTrigger();
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
	Super::InitiateDeath();
	DelegateManager->OnBossDeath.Broadcast();
}

void AMinionBoss::ToggleStompTrigger(const bool bState) const
{
	if (StompTriggerSphere)
	{
		StompTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		StompTriggerSphere->SetActorHiddenInGame(!bState);
	}
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

void AMinionBoss::OnPlayerEnterStompTrigger(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!AI || bAffectedByEmp) return;
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossStomp);
	ToggleStompTrigger(false);
	bStompIsReady = false;
}

void AMinionBoss::CheckDistanceOfLaserEyeTrigger()
{
	if (!AI || bAbilityInAction || bAffectedByEmp) return;
	const TObjectPtr<AActor> Target = AI->GetStateTreeData().GetTarget();
	if (!Target) return;
	if (FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) >= FMath::Square(1000.f))
	{
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossLaserEye);
		UKismetSystemLibrary::K2_ClearTimer(this, "CheckDistanceOfLaserEyeTrigger");
	}
}

void AMinionBoss::OnLaserCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor); Enemy->GetIsPossessed())
	{
		Player = Enemy;
		UKismetSystemLibrary::K2_SetTimer(this, "DealLaserDamageToPlayer", 0.05f, true);
	}
}

void AMinionBoss::OnLaserCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(OtherActor); Enemy->GetIsPossessed())
	{
		Player = nullptr;
		UKismetSystemLibrary::K2_ClearTimer(this, "DealLaserDamageToPlayer");
	}
}

void AMinionBoss::DealLaserDamageToPlayer() { if (Player) Player->ReceiveDamage(2.f, EDamageReason::MinionBossDamage); }
