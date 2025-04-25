#include "MinionBoss.h"

#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/TriggerSphere.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void AMinionBoss::AI_ToggleStompTrigger(const bool bState) const
{
	if (StompTriggerSphere)
	{
		StompTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		StompTriggerSphere->SetActorHiddenInGame(!bState);
	}
}

void AMinionBoss::TriggerStompCameraShake() const { (void)DelegateManager->OnMinionBossStomp.ExecuteIfBound(StompCameraShake); }

void AMinionBoss::EnableLaserEyeBall()
{
	if (!LaserEyeBallVfx) AssetsManager->AttachOrLoadVfx(EVfx::MinionBossLaserBall, &LaserEyeBallVfx, GetMesh(), "LaserSocket");
	else LaserEyeBallVfx->Activate();
}

void AMinionBoss::EnableLaser()
{
	if (!LaserVfx) AssetsManager->AttachOrLoadVfx(EVfx::MinionBossLaser, &LaserVfx, GetMesh(), "LaserSocket");
	else LaserVfx->Activate();
}

void AMinionBoss::EnableDistanceCheckForLaserEye() { UKismetSystemLibrary::K2_SetTimer(this, "CheckDistanceOfLaserEyeTrigger", 0.2f, true); }

AMinionBoss::AMinionBoss()
{
	Sounds.Add(ESound::MinionBossStomp, CreateDefaultSubobject<UAudioComponent>(TEXT("stompSfx")));
	Sounds[ESound::MinionBossStomp]->SetAutoActivate(false);
	Sounds[ESound::MinionBossStomp]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::MinionBossLaserEyeCharge, CreateDefaultSubobject<UAudioComponent>(TEXT("laserEyeChargeSfx")));
	Sounds[ESound::MinionBossLaserEyeCharge]->SetAutoActivate(false);
	Sounds[ESound::MinionBossLaserEyeCharge]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::MinionBossLaserEyeHold, CreateDefaultSubobject<UAudioComponent>(TEXT("laserEyeHoldSfx")));
	Sounds[ESound::MinionBossLaserEyeHold]->SetAutoActivate(false);
	Sounds[ESound::MinionBossLaserEyeHold]->SetupAttachment(GetMesh());
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

TObjectPtr<AWeapon>& AMinionBoss::GetWeapon() { return Weapons[0]; }

void AMinionBoss::BeginPlay()
{
	CharacterName = ECharacterType::MinionBoss;
	Super::BeginPlay();
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
	Cast<USphereComponent>(StompTriggerSphere->GetCollisionComponent())->SetSphereRadius(StompTriggerRadiusInMetres * 100.f);
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
	DestroyStompTriggerSphere();
	Super::InitiateDeath();
}

void AMinionBoss::OnPlayerEnterStompTrigger(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!AI) return;
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossStomp);
	AI_ToggleStompTrigger(false);
}

void AMinionBoss::CheckDistanceOfLaserEyeTrigger()
{
	if (!AI) return;
	const TObjectPtr<AActor> Target = AI->GetStateTreeData().GetTarget();
	if (!Target) return;
	if (FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) >= FMath::Square(1000.f))
	{
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::MinionBossLaserEye);
		UKismetSystemLibrary::K2_ClearTimer(this, "CheckDistanceOfLaserEyeTrigger");
	}
}
