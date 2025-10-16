#include "BreacherWeapon.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ABreacherWeapon::P_Shoot()
{
	AssetsManager->PlaySound2D(ESound::BreacherShoot);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	if (AnimInstance)
	{
		Mesh->SetComponentTickEnabled(true);
		AnimInstance->PlayMontage(EWeaponAnimState::Shoot);
	}
	Super::P_Shoot();
}

void ABreacherWeapon::P_Reload(const int32 InOtherAmmoSource)
{
	AssetsManager->PlaySound2D(ESound::BreacherReload);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	if (AnimInstance) AnimInstance->PlayMontage(EWeaponAnimState::Reload);
	Super::P_Reload(InOtherAmmoSource);
}

void ABreacherWeapon::P_Setup()
{
	Super::P_Setup();
	Mesh->SetComponentTickEnabled(true);
}

void ABreacherWeapon::BeginPlay()
{
	Name = EWeapon::BreacherWeapon;
	Super::BeginPlay();
	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
}
