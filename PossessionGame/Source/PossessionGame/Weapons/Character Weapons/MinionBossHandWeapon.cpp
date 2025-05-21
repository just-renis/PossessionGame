#include "MinionBossHandWeapon.h"

#include "Components/AudioComponent.h"

AMinionBossHandWeapon::AMinionBossHandWeapon()
{
	Sounds.Add(ESound::MinionBossShoot, CreateDefaultSubobject<UAudioComponent>(TEXT("shootSound")));
	Sounds[ESound::MinionBossShoot]->SetAutoActivate(false);
	Sounds[ESound::MinionBossShoot]->SetupAttachment(GetMesh());
}

void AMinionBossHandWeapon::AI_Shoot()
{
	PlaySound(ESound::MinionBossShoot, true);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::MinionBossProjectile);
}

void AMinionBossHandWeapon::BeginPlay()
{
	Name = EWeapon::MinionBossHandWeapon;
	Super::BeginPlay();
}

void AMinionBossHandWeapon::SetMeshes(const EWeapon InPrimaryDataAssetName)
{
	Super::SetMeshes(InPrimaryDataAssetName);
	Mesh->SetMaterial(0, MI_MinionBossWeaponMaterial);
}
