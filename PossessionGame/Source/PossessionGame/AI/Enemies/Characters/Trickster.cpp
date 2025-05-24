#include "Trickster.h"
#include "InputMappingContext.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/UI/MainWidgets/PlayerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/TricksterWeapon.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

ATrickster::ATrickster()
{
	Sounds.Add(ESound::TricksterSpawn, CreateDefaultSubobject<UAudioComponent>(TEXT("spawnSound")));
	Sounds[ESound::TricksterSpawn]->SetAutoActivate(false);
	Sounds[ESound::TricksterSpawn]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::TricksterDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("deathSound")));
	Sounds[ESound::TricksterDeath]->SetAutoActivate(false);
	Sounds[ESound::TricksterDeath]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::TricksterGrenadeThrow, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeThrowSound")));
	Sounds[ESound::TricksterGrenadeThrow]->SetAutoActivate(false);
	Sounds[ESound::TricksterGrenadeThrow]->SetupAttachment(GetMesh());
}

void ATrickster::ThrowPoisonGrenade()
{
	PlaySound(ESound::TricksterGrenadeThrow, true);
	Weapon->ShootProjectile(EPoolObject::TricksterPoisonProjectile);
}

void ATrickster::ThrowEmpGrenade()
{
	PlaySound(ESound::TricksterGrenadeThrow, true);
	Weapon->ShootProjectile(EPoolObject::TricksterEmpProjectile, false);
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

void ATrickster::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::TricksterOnPossession); }

void ATrickster::AI_PlaySpawnSound() { PlaySound(ESound::TricksterSpawn, true); }

void ATrickster::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::TricksterDeathElectrification, nullptr, FNiagaraVariationData(), nullptr, GetMesh(), "DeathSocket");
}

void ATrickster::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeThreeParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ATrickster::SetActionInMotion);
}

void ATrickster::P_SetupRightClickInputStartedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(14).Action, ETriggerEvent::Started, this, &ATrickster::StartedShootingWeapon, false);
}

void ATrickster::P_SetupRightClickInputTriggeredState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(14).Action, ETriggerEvent::Triggered, this, &ATrickster::P_Shoot, false);
}

void ATrickster::P_SetupRightClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(14).Action, ETriggerEvent::Completed, this, &ATrickster::StoppedShootingWeapon, false);
}

void ATrickster::P_SetupLeftClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Completed, this, &ATrickster::StoppedShootingWeapon, true);
}

void ATrickster::P_SetupLeftClickInputStartedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &ATrickster::StartedShootingWeapon, true);
}

void ATrickster::P_SetupColorsForUI() { P_HUD->SetColors(FLinearColor(0.f, 0.57255f, 0.58431f)); }

void ATrickster::BeginPlay()
{
	CharacterName = ECharacterType::Trickster;
	Super::BeginPlay();
}

void ATrickster::StartedShootingWeapon(bool bFirstWeapon) { (void)OnWeaponShootStateChange.ExecuteIfBound(bFirstWeapon, true); }

void ATrickster::StoppedShootingWeapon(bool bFirstWeapon) { (void)OnWeaponShootStateChange.ExecuteIfBound(bFirstWeapon, false); }
