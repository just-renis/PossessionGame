#include "Ranger.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_VfxContainer.h"
#include "PossessionGame/Destructibles/RangerShield.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/UI/PlayerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Animation Instances/Hands/RangerHandsAnimInstance.h"
#include "PossessionGame/Weapons/Character Weapons/RangerWeapon.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

ARanger::ARanger()
{
	Sounds.Add(ESound::RangerSpawn, CreateDefaultSubobject<UAudioComponent>(TEXT("spawnSound")));
	Sounds[ESound::RangerSpawn]->SetAutoActivate(false);
	Sounds.Add(ESound::RangerDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("deathSound")));
	Sounds[ESound::RangerDeath]->SetAutoActivate(false);
	Sounds.Add(ESound::RangerGrenadeLaunch, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeLaunchSound")));
	Sounds[ESound::RangerGrenadeLaunch]->SetAutoActivate(false);
	Sounds.Add(ESound::RangerGrenadeLaunchAim, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeLaunchAimSound")));
	Sounds[ESound::RangerGrenadeLaunchAim]->SetAutoActivate(false);
}

void ARanger::LaunchGrenade()
{
	PlaySound(ESound::RangerGrenadeLaunch);
	Weapon->ShootProjectile(EPoolObject::RangerGrenadeLauncherProjectile);
	GrenadeLauncherMuzzleFlashLogic();
}

void ARanger::SpawnShield()
{
	const TObjectPtr<APoolObject> Shield = GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(bIsPossessed, EPoolObject::RangerShield, GetActorLocation() + GetActorForwardVector() * 100.f, GetActorRotation() + FRotator(0.f, -90.f, 0.f));
	Cast<ARangerShield>(Shield)->SetProprietor(this);
}

void ARanger::SetCustomTimeDilation(float Dilation)
{
	if (FMath::IsNearlyEqual(Dilation, 5.f, 0.1f))
	{
		if (AssetsManager->GetVfxData(EVfx::StopTimeAbility)->GetVfxEffect().IsValid()) SpawnStopTimeAbilityVfx(EVfx::StopTimeAbility);
		else AssetsManager->GetVfxData(EVfx::StopTimeAbility)->LoadVfx(GetWorld(), [this] { SpawnStopTimeAbilityVfx(EVfx::StopTimeAbility); });
	}
	else if (AssetsManager->GetVfxData(EVfx::StopTimeAbilityReverse)->GetVfxEffect().IsValid()) SpawnStopTimeAbilityVfx(EVfx::StopTimeAbilityReverse);
	else AssetsManager->GetVfxData(EVfx::StopTimeAbilityReverse)->LoadVfx(GetWorld(), [this] { SpawnStopTimeAbilityVfx(EVfx::StopTimeAbilityReverse); });
	CustomTimeDilation = Dilation;
	GetWeapon()->CustomTimeDilation = Dilation;
}

void ARanger::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::RangerOnPossession); }

void ARanger::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::RangerWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ARanger::AI_PlayDeathSound() { PlaySound(ESound::RangerDeath); }

void ARanger::AI_PlaySpawnSound() { PlaySound(ESound::RangerSpawn); }

void ARanger::P_Setup()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ARanger::SetActionInMotion);
}

void ARanger::P_SetupRightClickInputStartedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Started, this, &ARanger::P_ChangeAiming, true);
}

void ARanger::P_SetupRightClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Completed, this, &ARanger::P_ChangeAiming, false);
}

void ARanger::P_ChangeAiming(bool bIsZooming) { Cast<ARangerWeapon>(Weapon)->P_ZoomToCamera(bIsZooming, MouseSensitivity); }

void ARanger::P_SetupColorsForUI() { P_HUD->SetColors(FLinearColor(0.58431f, 0.11373f, 0.f)); }

void ARanger::BeginPlay()
{
	CharacterName = ECharacterType::Ranger;
	Super::BeginPlay();
	AttachOrLoadGrenadeLauncherMuzzle();
	UKismetSystemLibrary::K2_SetTimer(this, "AI_PlaySpawnSound", 3.f + UKismetMathLibrary::RandomInteger(4), false);
}

void ARanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsPossessed) Cast<ARangerWeapon>(Weapon)->ZoomLogic(Camera, DeltaTime);
}

void ARanger::SpawnStopTimeAbilityVfx(EVfx VfxName)
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(AssetsManager->GetVfxData(VfxName)->GetVfxEffect().Get(), GetCamera(), NAME_None, FVector(20.f, 0.f, 0.f), FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
}

void ARanger::AttachOrLoadGrenadeLauncherMuzzle()
{
	if (AssetsManager->GetVfxData(EVfx::RangerGrenadeLauncherMuzzle)->GetVfxEffect().IsValid()) AttachGrenadeLauncherMuzzle();
	else AssetsManager->GetVfxData(EVfx::RangerGrenadeLauncherMuzzle)->LoadVfx(GetWorld(), [this] { AttachGrenadeLauncherMuzzle(); });
}

void ARanger::AttachGrenadeLauncherMuzzle()
{
	GrenadeLauncherMuzzleFlashComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(AssetsManager->GetVfxData(EVfx::RangerGrenadeLauncherMuzzle)->GetVfxEffect().Get(), Weapon->GetMesh(), FName("GrenadeLauncherMuzzleSocket"),
	FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	GrenadeLauncherMuzzleFlashComponent->Deactivate();
}

void ARanger::GrenadeLauncherMuzzleFlashLogic()
{
	if (GrenadeLauncherMuzzleFlashComponent)
	{
		if (GrenadeLauncherMuzzleFlashComponent->IsActive()) GrenadeLauncherMuzzleFlashComponent->Deactivate();
		GrenadeLauncherMuzzleFlashComponent->Activate();
	}
}

float ARanger::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ReturnDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (!bIsPossessed && CurrentHp <= MaxHp * 0.5f && CanAbilityBeExecuted(1)) AI_UseAbility(1);
	return ReturnDamage;
}

void ARanger::LoadSounds()
{
	Sounds[ESound::RangerSpawn]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Sounds[ESound::RangerDeath]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Sounds[ESound::RangerGrenadeLaunch]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Sounds[ESound::RangerGrenadeLaunchAim]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ARanger::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (bIsPossessed) UKismetSystemLibrary::K2_ClearTimer(this, "AI_PlaySpawnSound");
}
