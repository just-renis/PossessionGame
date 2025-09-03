#include "Trickster.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/UI/MainWidgets/PlayerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/TricksterWeapon.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

ATrickster::ATrickster()
{
	AI_PoisonGrenadeLaunchTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("PoisonGrenadeLaunchTrigger"));
	AI_PoisonGrenadeLaunchTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	AI_PoisonGrenadeLaunchTrigger->SetCollisionProfileName("TriggerPlayer");
	AI_PoisonGrenadeLaunchTrigger->CanCharacterStepUpOn = ECB_No;
	AI_PoisonGrenadeLaunchTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AI_PoisonGrenadeLaunchTrigger->SetSphereRadius(PoisonGrenadeLaunchTriggerRadiusInMetres * 100.f);
	AI_PoisonGrenadeLaunchTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATrickster::AI_OnPlayerEnterPoisonGrenadeLaunchTrigger);
	Sounds.Add(ESound::TricksterSpawn, CreateDefaultSubobject<UAudioComponent>(TEXT("spawnSound")));
	Sounds[ESound::TricksterSpawn]->SetAutoActivate(false);
	Sounds[ESound::TricksterSpawn]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::TricksterDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("deathSound")));
	Sounds[ESound::TricksterDeath]->SetAutoActivate(false);
	Sounds[ESound::TricksterDeath]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::TricksterGrenadeThrow, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeThrowSound")));
	Sounds[ESound::TricksterGrenadeThrow]->SetAutoActivate(false);
	Sounds[ESound::TricksterGrenadeThrow]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::RangerGrenadeLaunchAim, CreateDefaultSubobject<UAudioComponent>(TEXT("PoisonGrenadeLaunchAimSound")));
	Sounds[ESound::RangerGrenadeLaunchAim]->SetAutoActivate(false);
	Sounds[ESound::RangerGrenadeLaunchAim]->SetupAttachment(GetMesh());
}

void ATrickster::AI_TogglePoisonGrenadeLaunchTrigger(const bool bState) const
{
	if (AI_PoisonGrenadeLaunchTrigger) AI_PoisonGrenadeLaunchTrigger->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ATrickster::AI_LaunchPoisonGrenade()
{
	PlaySound(ESound::TricksterGrenadeThrow, true);
	Weapon->ShootProjectile(EPoolObject::TricksterPoisonProjectile, true);
	//GrenadeLauncherMuzzleFlashLogic();
}

void ATrickster::P_ThrowPoisonGrenade() const
{
	AssetsManager->PlaySound2D(ESound::TricksterGrenadeThrow);
	Weapon->ShootProjectile(EPoolObject::TricksterPoisonProjectile, true);
}

void ATrickster::P_ThrowEmpGrenade() const
{
	AssetsManager->PlaySound2D(ESound::TricksterGrenadeThrow);
	Weapon->ShootProjectile(EPoolObject::TricksterEmpProjectile, true, false);
}

void ATrickster::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::TricksterWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ATrickster::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::TricksterDeath);
}

void ATrickster::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::TricksterOnHijack); }

void ATrickster::AI_PlaySpawnSound() { PlaySound(ESound::TricksterSpawn, true); }

void ATrickster::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::TricksterDeathElectrification, nullptr, nullptr, GetMesh(), "DeathSocket");
}

void ATrickster::AI_TryTriggeringPoisonGrenadeLaunch()
{
	if (!AI || AnimInstance->IsPlaying(EEnemyAnimState::Reload)) return;
	if (AI->GetStateTreeData().GetIsTargetInMySight())
	{
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::TricksterLaunchPoisonGrenade);
		GetWorld()->GetTimerManager().ClearTimer(TryTriggeringPoisonGrenadeLaunchHandle);
	}
}

void ATrickster::AI_OnPlayerEnterPoisonGrenadeLaunchTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("AliveEnemy")) return;
	AI_TogglePoisonGrenadeLaunchTrigger(false);
	GetWorld()->GetTimerManager().SetTimer(TryTriggeringPoisonGrenadeLaunchHandle, this, &ATrickster::AI_TryTriggeringPoisonGrenadeLaunch, 0.1f, true);
}

void ATrickster::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ATrickster::SetIsActionInMotion);
}

void ATrickster::P_SetupColorsForUI() { P_HUD->SetColors(FLinearColor(0.f, 0.57255f, 0.58431f)); }

void ATrickster::BeginPlay()
{
	CharacterName = ECharacterType::Trickster;
	Super::BeginPlay();
}
