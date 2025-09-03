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
#include "PossessionGame/UI/MainWidgets/PlayerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Animation Instances/Hands/RangerHandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

ARanger::ARanger()
{
	AI_GrenadeLaunchTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("GrenadeLaunchTrigger"));
	AI_GrenadeLaunchTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	AI_GrenadeLaunchTrigger->SetCollisionProfileName("TriggerPlayer");
	AI_GrenadeLaunchTrigger->CanCharacterStepUpOn = ECB_No;
	AI_GrenadeLaunchTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AI_GrenadeLaunchTrigger->SetSphereRadius(GrenadeLaunchTriggerRadiusInMetres * 100.f);
	AI_GrenadeLaunchTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARanger::AI_OnPlayerEnterGrenadeLaunchTrigger);
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

void ARanger::AI_ToggleGrenadeLaunchTrigger(const bool bState) const
{
	if (AI_GrenadeLaunchTrigger) AI_GrenadeLaunchTrigger->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ARanger::AI_LaunchGrenade()
{
	PlaySound(ESound::RangerGrenadeLaunch, true);
	Weapon->ShootProjectile(EPoolObject::RangerGrenadeLauncherProjectile, true);
	GrenadeLauncherMuzzleFlashLogic();
}

void ARanger::P_LaunchGrenade()
{
	AssetsManager->PlaySound2D(ESound::RangerGrenadeLaunch);
	Weapon->ShootProjectile(EPoolObject::RangerGrenadeLauncherProjectile, true);
	GrenadeLauncherMuzzleFlashLogic();
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

void ARanger::AI_DestroyGrenadeLaunchTriggerSphere() const
{
	if (AI_GrenadeLaunchTrigger)
	{
		AI_GrenadeLaunchTrigger->OnComponentBeginOverlap.RemoveAll(this);
		AI_GrenadeLaunchTrigger->DestroyComponent();
	}
}

void ARanger::AI_TryTriggeringGrenadeLaunch()
{
	if (!AI || AnimInstance->IsPlaying(EEnemyAnimState::Reload)) return;
	if (AI->GetStateTreeData().GetIsTargetInMySight())
	{
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::RangerLaunchGrenade);
		GetWorld()->GetTimerManager().ClearTimer(TryTriggeringGrenadeLaunchHandle);
	}
}

void ARanger::AI_OnPlayerEnterGrenadeLaunchTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("AliveEnemy")) return;
	AI_ToggleGrenadeLaunchTrigger(false);
	GetWorld()->GetTimerManager().SetTimer(TryTriggeringGrenadeLaunchHandle, this, &ARanger::AI_TryTriggeringGrenadeLaunch, 0.1f, true);
}

void ARanger::P_Setup()
{
	GetWorld()->GetTimerManager().ClearTimer(TryTriggeringGrenadeLaunchHandle);
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	P_ChangeRangerVariationMuzzle();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ARanger::SetIsActionInMotion);
}

void ARanger::P_SetupColorsForUI() { P_HUD->SetColors(FLinearColor(0.58431f, 0.11373f, 0.f)); }

void ARanger::P_ChangeRangerVariationMuzzle()
{
	if (AI_GrenadeLauncherMuzzleFlashComponent)
	{
		AI_GrenadeLauncherMuzzleFlashComponent->DeactivateImmediate();
		AI_GrenadeLauncherMuzzleFlashComponent = nullptr;
	}
}

void ARanger::BeginPlay()
{
	CharacterName = ECharacterType::Ranger;
	Super::BeginPlay();
}

void ARanger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(TryTriggeringGrenadeLaunchHandle);
	Super::EndPlay(EndPlayReason);
}

void ARanger::GrenadeLauncherMuzzleFlashLogic()
{
	if (!AI_GrenadeLauncherMuzzleFlashComponent)
	{
		AssetsManager->AttachVfx(EVfx::RangerGrenadeLauncherMuzzle, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &AI_GrenadeLauncherMuzzleFlashComponent,
		Weapon->GetMesh(), FName("GrenadeLauncherMuzzleSocket"), true,FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, GetVariation());
	}
	else AI_GrenadeLauncherMuzzleFlashComponent->Activate(true);
}

float ARanger::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ReturnDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (!bIsActionInMotion && !bIsHijacked && CurrentHp <= MaxHp * 0.5f && CanAbilityBeExecuted(1)) AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::RangerDeployShield);
	return ReturnDamage;
}

void ARanger::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AI_DestroyGrenadeLaunchTriggerSphere();
}

void ARanger::InitiateDeath()
{
	AI_DestroyGrenadeLaunchTriggerSphere();
	Super::InitiateDeath();
}
