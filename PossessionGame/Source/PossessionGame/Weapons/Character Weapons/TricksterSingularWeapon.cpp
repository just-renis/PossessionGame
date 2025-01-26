#include "TricksterSingularWeapon.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void ATricksterSingularWeapon::AI_Shoot()
{
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::TricksterProjectile);
}

bool ATricksterSingularWeapon::P_CanReload(int32 OtherAmmoSource)
{
	return OtherAmmoSource != -1 ? OtherAmmoSource > 0 : TotalAmmo > 0 && CurrentAmmo != MagazineCap;
}

void ATricksterSingularWeapon::P_Shoot(bool bIsFirstWeapon)
{
	if (ShootTimer >= ShootCooldown)
	{
		MuzzleFlashLogic();
		MakeShootingCameraShake();
		CurrentAmmo--;
		ShootTimer = 0.f;
		bIsHitScanEnabled ? HitScanLogic() : ShootProjectile(EPoolObject::RangerProjectile);
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
