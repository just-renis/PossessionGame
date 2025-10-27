#include "SamuraiWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ASamuraiWeapon::P_Shoot()
{
	//PlayHandsAndWeaponMontage(EHandsAnimState::Shoot, EWeaponAnimState::Shoot);
	Super::P_Shoot();
}

void ASamuraiWeapon::P_UpdateAmmoTextHUD() { DelegateManager->OnWeaponAmmoChangeOneParam.Broadcast(CurrentAmmo); }

void ASamuraiWeapon::BeginPlay()
{
	Name = EWeapon::SamuraiWeapon;
	Super::BeginPlay();
//	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
//	AnimInstance->SetWeaponName(Name);
}

void ASamuraiWeapon::ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, const bool bFromFirstWeapon) { Super::ShootProjectile(EPoolObject::SamuraiProjectile, false); }
