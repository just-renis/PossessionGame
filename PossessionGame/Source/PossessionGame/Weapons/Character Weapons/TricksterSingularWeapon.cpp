#include "TricksterSingularWeapon.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ATricksterSingularWeapon::ATricksterSingularWeapon()
{
	Sounds.Add(ESound::TricksterShoot, CreateDefaultSubobject<UAudioComponent>(TEXT("shootSound")));
	Sounds[ESound::TricksterShoot]->SetAutoActivate(false);
	Sounds[ESound::TricksterShoot]->SetupAttachment(GetMesh());
}

void ATricksterSingularWeapon::AI_Shoot()
{
	PlaySound(ESound::TricksterShoot, true);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::TricksterProjectile, false);
}

bool ATricksterSingularWeapon::P_CanReload(int32 OtherAmmoSource)
{
	return (OtherAmmoSource != -1 ? OtherAmmoSource > 0 : TotalAmmo > 0) && CurrentAmmo != MagazineCap;
}

void ATricksterSingularWeapon::P_Shoot(bool bIsFirstWeapon)
{
	if (ShootTimer >= ShootCooldown)
	{
		PlaySound(ESound::TricksterShoot, true);
		MuzzleFlashLogic();
		MakeShootingCameraShake();
		CurrentAmmo--;
		ShootTimer = 0.f;
		bIsHitScanEnabled ? HitScanLogic() : ShootProjectile(EPoolObject::TricksterProjectile);
	}
}

void ATricksterSingularWeapon::BeginPlay()
{
	Name = EWeapon::TricksterSingularWeapon;
	Super::BeginPlay();
}

void ATricksterSingularWeapon::SetMeshes(EWeapon PrimaryDataAssetName)
{
	if (!Mesh->GetSkeletalMeshAsset()) Mesh->SetSkeletalMesh(GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->GetWeaponData(PrimaryDataAssetName)->WeaponMesh.Get());
	OriginalScale = Mesh->GetRelativeScale3D();
}
