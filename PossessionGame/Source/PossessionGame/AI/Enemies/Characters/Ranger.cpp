#include "Ranger.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/TriggerSphere.h"
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
	if (AIGrenadeLaunchTriggerSphere) AIGrenadeLaunchTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
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

void ARanger::AI_OnPlayerEnterGrenadeLaunchTrigger(AActor* OverlappedActor, AActor* OtherActor)
{
	AI_ToggleGrenadeLaunchTrigger(false);
	GetWorld()->GetTimerManager().SetTimer(TryTriggeringGrenadeLaunchHandle, this, &ARanger::TryTriggeringGrenadeLaunch, 0.1f, true);
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
	if (GrenadeLauncherMuzzleFlashComponent)
	{
		GrenadeLauncherMuzzleFlashComponent->DeactivateImmediate();
		GrenadeLauncherMuzzleFlashComponent = nullptr;
	}
}

void ARanger::BeginPlay()
{
	CharacterName = ECharacterType::Ranger;
	Super::BeginPlay();
	SetupGrenadeLaunchTriggerSphere();
}

void ARanger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(TryTriggeringGrenadeLaunchHandle);
	Super::EndPlay(EndPlayReason);
}

void ARanger::GrenadeLauncherMuzzleFlashLogic()
{
	if (!GrenadeLauncherMuzzleFlashComponent)
	{
		AssetsManager->AttachVfx(EVfx::RangerGrenadeLauncherMuzzle, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &GrenadeLauncherMuzzleFlashComponent,
		Weapon->GetMesh(), FName("GrenadeLauncherMuzzleSocket"), true,FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, GetVariation());
	}
	else GrenadeLauncherMuzzleFlashComponent->Activate(true);
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
	DestroyGrenadeLaunchTriggerSphere();
}

void ARanger::InitiateDeath()
{
	DestroyGrenadeLaunchTriggerSphere();
	Super::InitiateDeath();
}

void ARanger::SetupGrenadeLaunchTriggerSphere()
{
	AIGrenadeLaunchTriggerSphere = GetWorld()->SpawnActor<ATriggerSphere>(ATriggerSphere::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	AIGrenadeLaunchTriggerSphere->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	AIGrenadeLaunchTriggerSphere->GetCollisionComponent()->SetCollisionProfileName("TriggerPlayer");
	AIGrenadeLaunchTriggerSphere->GetCollisionComponent()->CanCharacterStepUpOn = ECB_No;
	AIGrenadeLaunchTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Cast<USphereComponent>(AIGrenadeLaunchTriggerSphere->GetCollisionComponent())->SetSphereRadius(GrenadeLaunchTriggerRadiusInMetres * 100.f);
	AIGrenadeLaunchTriggerSphere->OnActorBeginOverlap.AddDynamic(this, &ARanger::AI_OnPlayerEnterGrenadeLaunchTrigger);
}

void ARanger::DestroyGrenadeLaunchTriggerSphere() const
{
	if (AIGrenadeLaunchTriggerSphere)
	{
		AIGrenadeLaunchTriggerSphere->OnActorBeginOverlap.RemoveAll(this);
		AIGrenadeLaunchTriggerSphere->Destroy();
	}
}

void ARanger::TryTriggeringGrenadeLaunch()
{
	if (!AI || AnimInstance->IsPlaying(EEnemyAnimState::Reload)) return;
	if (AI->GetStateTreeData().GetIsTargetInMySight())
	{
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::RangerLaunchGrenade);
		GetWorld()->GetTimerManager().ClearTimer(TryTriggeringGrenadeLaunchHandle);
	}
}
