#include "Trickster.h"
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

void ATrickster::P_ThrowPoisonGrenade() const
{
	AssetsManager->PlaySound2D(ESound::TricksterGrenadeThrow);
	Weapon->ShootProjectile(EPoolObject::TricksterPoisonProjectile);
}

void ATrickster::P_ThrowEmpGrenade() const
{
	AssetsManager->PlaySound2D(ESound::TricksterGrenadeThrow);
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

void ATrickster::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::TricksterOnHijack); }

void ATrickster::AI_PlaySpawnSound() { PlaySound(ESound::TricksterSpawn, true); }

void ATrickster::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::TricksterDeathElectrification, nullptr, nullptr, GetMesh(), "DeathSocket");
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
