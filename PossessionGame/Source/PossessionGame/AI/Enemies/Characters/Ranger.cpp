#include "Ranger.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Destructibles/RangerShield.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EStateTreeEvent.h"
#include "PossessionGame/Projectiles/Throwables/RangerMine.h"
#include "PossessionGame/UI/MainWidgets/PlayerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Animation Instances/Hands/RangerHandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

ARanger::ARanger()
{
	AI_GrenadeTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("GrenadeLaunchTrigger"));
	AI_GrenadeTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	AI_GrenadeTrigger->SetCollisionProfileName("TriggerPlayer");
	AI_GrenadeTrigger->CanCharacterStepUpOn = ECB_No;
	AI_GrenadeTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AI_GrenadeTrigger->SetSphereRadius(GrenadeTriggerRadiusInMetres * 100.f);
	AI_GrenadeTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARanger::AI_OnPlayerEnterGrenadeTrigger);
	Sounds.Add(ESound::RangerSpawn, CreateDefaultSubobject<UAudioComponent>(TEXT("spawnSound")));
	Sounds[ESound::RangerSpawn]->SetAutoActivate(false);
	Sounds[ESound::RangerSpawn]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::RangerDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("deathSound")));
	Sounds[ESound::RangerDeath]->SetAutoActivate(false);
	Sounds[ESound::RangerDeath]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::RangerGrenadeLaunch, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeLaunchSound")));
	Sounds[ESound::RangerGrenadeLaunch]->SetAutoActivate(false);
	Sounds[ESound::RangerGrenadeLaunch]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::RangerGrenadeLaunchAim, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeLaunchAimSound")));
	Sounds[ESound::RangerGrenadeLaunchAim]->SetAutoActivate(false);
	Sounds[ESound::RangerGrenadeLaunchAim]->SetupAttachment(GetMesh());
}

void ARanger::AI_ToggleGrenadeTrigger(const bool bState) const
{
	if (AI_GrenadeTrigger) AI_GrenadeTrigger->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ARanger::AI_LaunchGrenade()
{
	PlaySound(ESound::RangerGrenadeLaunch, true);
	Weapon->ShootProjectile(EPoolObject::RangerGrenadeLauncherProjectile, true);
	GrenadeMuzzleFlashLogic();
}

void ARanger::P_LaunchGrenade()
{
	AssetsManager->PlaySound2D(ESound::RangerGrenadeLaunch);
	Weapon->ShootProjectile(EPoolObject::RangerGrenadeLauncherProjectile, true);
	GrenadeMuzzleFlashLogic();
}

void ARanger::P_InitiateThrowMine()
{
	Mine = Cast<ARangerMine>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::RangerMine));
	Mine->Init();
	Mine->AttachToComponent(Weapon->GetHandsMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("MineSocket"));
	GetWorldTimerManager().SetTimer(MineThrowHandle, this, &ARanger::P_ThrowMine, 0.46f, false);
	UE_LOG(LogTemp, Error, TEXT("P_InitiateThrowMine"));
}

void ARanger::SpawnShield()
{
	const TObjectPtr<APoolObject> Shield = GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::RangerShield, GetActorLocation() + GetActorForwardVector() * 100.f, GetActorRotation() + FRotator(0.f, -90.f, 0.f));
	Shield->Init(bIsHijacked);
	Cast<ARangerShield>(Shield)->SetProprietor(this);
}

void ARanger::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::RangerOnHijack); }

void ARanger::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::RangerWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ARanger::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::RangerDeath);
}

void ARanger::AI_PlaySpawnSound() { PlaySound(ESound::RangerSpawn, true); }

void ARanger::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::RangerDeathElectrification, nullptr, nullptr, GetMesh(), "DeathSocket");
}

void ARanger::AI_DestroyGrenadeTrigger() const
{
	if (AI_GrenadeTrigger)
	{
		AI_GrenadeTrigger->OnComponentBeginOverlap.RemoveAll(this);
		AI_GrenadeTrigger->DestroyComponent();
	}
}

void ARanger::AI_TryTriggeringGrenade()
{
	if (!AI || AnimInstance->IsPlaying(EEnemyAnimState::Reload)) return;
	if (AI->GetStateTreeData().GetIsTargetInMySight())
	{
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::RangerLaunchGrenade);
		GetWorld()->GetTimerManager().ClearTimer(TryTriggeringGrenadeHandle);
	}
}

void ARanger::AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage)
{
	if (CanAbilityBeExecuted(1) && !bAffectedByEmp && !bIsActionInMotion && CurrentHp <= MaxHp * 0.5f) AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::RangerDeployShield);
	Super::AI_ReceivedDamage(InDamageReason, InDamage);
}

void ARanger::AI_OnPlayerEnterGrenadeTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bAffectedByEmp || !OtherActor->ActorHasTag("Player")) return;
	AI_ToggleGrenadeTrigger(false);
	GetWorld()->GetTimerManager().SetTimer(TryTriggeringGrenadeHandle, this, &ARanger::AI_TryTriggeringGrenade, 0.1f, true);
}

void ARanger::P_Setup()
{
	GetWorld()->GetTimerManager().ClearTimer(TryTriggeringGrenadeHandle);
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	P_ChangeRangerVariationMuzzle();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ARanger::SetIsActionInMotion);
}

void ARanger::P_SetupColorsForUI() { P_HUD->SetColors(FLinearColor(0.58431f, 0.11373f, 0.f)); }

void ARanger::P_ChangeRangerVariationMuzzle()
{
	if (AI_GrenadeMuzzleFlashComponent)
	{
		AI_GrenadeMuzzleFlashComponent->DeactivateImmediate();
		AI_GrenadeMuzzleFlashComponent = nullptr;
	}
}

void ARanger::P_ThrowMine()
{
	Mine->Throw(GetActorForwardVector(), 2000.f);
	UE_LOG(LogTemp, Error, TEXT("P_ThrowMine"));
}

void ARanger::BeginPlay()
{
	CharacterName = ECharacterType::Ranger;
	Super::BeginPlay();
}

void ARanger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(TryTriggeringGrenadeHandle);
	Super::EndPlay(EndPlayReason);
}

void ARanger::GrenadeMuzzleFlashLogic()
{
	if (!AI_GrenadeMuzzleFlashComponent)
	{
		AssetsManager->AttachVfx(EVfx::RangerGrenadeLauncherMuzzle, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &AI_GrenadeMuzzleFlashComponent,
		Weapon->GetMesh(), FName("GrenadeLauncherMuzzleSocket"), true,FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, GetVariation());
	}
	else AI_GrenadeMuzzleFlashComponent->Activate(true);
}

void ARanger::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && NewController->IsPlayerController()) AI_DestroyGrenadeTrigger();
}

void ARanger::InitiateDeath()
{
	AI_DestroyGrenadeTrigger();
	Super::InitiateDeath();
}

void ARanger::SetEmpEffect(const float InEffectDuration)
{
	switch (GetAI()->GetCurrentStateTreeEvent())
	{
		case EStateTreeEvent::RangerLaunchGrenade: AI_StopAbility(0); break;
		case EStateTreeEvent::RangerDeployShield: AI_StopAbility(1); break;
		default: ;
	}
	Super::SetEmpEffect(InEffectDuration);
}

void ARanger::EmpEffectEnded()
{
	Super::EmpEffectEnded();
	if (!AI_GrenadeTrigger || AI_GrenadeTrigger->GetCollisionEnabled() != ECollisionEnabled::QueryOnly) return;
	AI_GrenadeTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AI_GrenadeTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
