#include "RangerWeapon.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

ARangerWeapon::ARangerWeapon()
{
	Sounds.Add(ESound::RangerShoot, CreateDefaultSubobject<UAudioComponent>(TEXT("shootSound")));
	Sounds[ESound::RangerShoot]->SetAutoActivate(false);
	Sounds[ESound::RangerShoot]->SetupAttachment(Mesh);
	Sounds.Add(ESound::RangerReload, CreateDefaultSubobject<UAudioComponent>(TEXT("reloadSound")));
	Sounds[ESound::RangerReload]->SetAutoActivate(false);
	Sounds[ESound::RangerReload]->SetupAttachment(Mesh);
}

void ARangerWeapon::AI_Reload() { PlaySound(ESound::RangerReload, true); }

void ARangerWeapon::AI_Shoot()
{
	PlaySound(ESound::RangerShoot, true);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::RangerProjectile);
}

void ARangerWeapon::P_Shoot()
{
	AssetsManager->PlaySound2D(ESound::RangerShoot);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	Super::P_Shoot();
}

void ARangerWeapon::P_Reload(int32 OtherAmmoSource)
{
	AssetsManager->PlaySound2D(ESound::RangerReload);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	if (AnimInstance) AnimInstance->PlayMontage(EWeaponAnimState::Reload);
	Super::P_Reload(OtherAmmoSource);
}

void ARangerWeapon::BeginPlay()
{
	Name = EWeapon::RangerWeapon;
	Super::BeginPlay();
	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
}
