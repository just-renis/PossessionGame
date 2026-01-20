#include "Wraith.h"

#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Projectiles/Throwables/WraithMeteorTrigger.h"
#include "PossessionGame/Projectiles/Throwables/WraithMolotov.h"
#include "PossessionGame/Projectiles/Throwables/WraithTrap.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void AWraith::P_InitiateThrowMolotov()
{
	const TObjectPtr<AWraithMolotov> Molotov = Cast<AWraithMolotov>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithMolotov, Weapon->GetHandsSocketLocation(FName("ThrowSocket")), Weapon->GetHandsSocketRotation(FName("ThrowSocket"))));
	if (!Molotov) return;
	Molotov->Init(this);
	AssetsManager->PlaySound2D(ESound::WraithThrow);
	Molotov->Throw();
}

void AWraith::P_InitiateThrowTrap()
{
	const TObjectPtr<AWraithTrap> Trap = Cast<AWraithTrap>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithTrap, Weapon->GetHandsSocketLocation(FName("ThrowSocket")), Weapon->GetHandsSocketRotation(FName("ThrowSocket"))));
	if (!Trap) return;
	Trap->Init(this);
	AssetsManager->PlaySound2D(ESound::WraithThrow);
	Trap->Throw();
}

void AWraith::P_InitiateThrowMeteorShower()
{
	const TObjectPtr<AWraithMeteorTrigger> MeteorTrigger = Cast<AWraithMeteorTrigger>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithMeteorTrigger, Weapon->GetHandsSocketLocation(FName("ThrowSocket")), Weapon->GetHandsSocketRotation(FName("ThrowSocket"))));
	if (!MeteorTrigger) return;
	MeteorTrigger->Init(this);
	AssetsManager->PlaySound2D(ESound::WraithThrow);
	MeteorTrigger->Throw();
}

AWraith::AWraith()
{
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSfx"));
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(GetMesh());
	DeathSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathSfx"));
	DeathSfx->SetAutoActivate(false);
	DeathSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::WraithSpawn, SpawnSfx);
	Sounds.Add(ESound::WraithDeath, DeathSfx);
}

void AWraith::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::WraithWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void AWraith::AI_PlaySpawnSound() { PlaySound(ESound::WraithSpawn, true); }

void AWraith::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::WraithDeath);
}

void AWraith::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::WraithOnHijack); }

void AWraith::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::WraithDeathElectrification, nullptr, nullptr, GetMesh(), "DeathSocket");
}

void AWraith::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &AWraith::SetIsActionInMotion);
}

void AWraith::P_WeaponShoot() { Weapon->P_Shoot(EPoolObject::WraithProjectile); }

void AWraith::BeginPlay()
{
	CharacterName = ECharacterType::Wraith;
	Super::BeginPlay();
}
