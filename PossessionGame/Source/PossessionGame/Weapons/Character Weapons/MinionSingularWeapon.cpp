#include "MinionSingularWeapon.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AMinionSingularWeapon::AMinionSingularWeapon()
{
	Sounds.Add(ESound::MinionShoot, CreateDefaultSubobject<UAudioComponent>(TEXT("shootSound")));
	Sounds[ESound::MinionShoot]->SetAutoActivate(false);
	Sounds[ESound::MinionShoot]->SetupAttachment(GetMesh());
}

void AMinionSingularWeapon::AI_Shoot()
{
	PlaySound(ESound::MinionShoot, true);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::MinionProjectile);
}

void AMinionSingularWeapon::BeginPlay()
{
	Name = EWeapon::MinionSingularWeapon;
	Super::BeginPlay();
}

void AMinionSingularWeapon::SetMeshes(EWeapon PrimaryDataAssetName)
{
	if (!Mesh->GetSkeletalMeshAsset()) Mesh->SetSkeletalMesh(GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->GetWeaponData(PrimaryDataAssetName)->WeaponMesh.Get());
	OriginalScale = Mesh->GetRelativeScale3D();
}
