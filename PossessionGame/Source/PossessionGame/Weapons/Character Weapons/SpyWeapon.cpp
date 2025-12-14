#include "SpyWeapon.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ASpyWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	Super::P_Shoot(InProjectileName);
}

void ASpyWeapon::P_Reload()
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	if (AnimInstance) AnimInstance->PlayMontage(EWeaponAnimState::Reload);
}

void ASpyWeapon::P_Setup()
{
	Super::P_Setup();
	Mesh->SetComponentTickEnabled(true);
}

void ASpyWeapon::BeginPlay()
{
	Name = EWeapon::SpyWeapon;
	Super::BeginPlay();
	AnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance());
	AnimInstance->SetWeaponName(Name);
}

void ASpyWeapon::ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, const bool bFromFirstWeapon)
{
	Super::ShootProjectile(InProjectileName, bIsFromAbility);
	if (CurrentAmmo <= MagazineCap * 0.1f) OnAmmoPercentageChanged.Broadcast(10);
	else if (CurrentAmmo <= MagazineCap * 0.5f) OnAmmoPercentageChanged.Broadcast(50);
}

void ASpyWeapon::SetConceal(bool bState)
{
	Mesh->SetScalarParameterValueOnMaterials("Opacity", bState ? 0.2f : 1.f);
	HandsMesh->SetScalarParameterValueOnMaterials("Opacity", bState ? 0.2f : 1.f);
}

void ASpyWeapon::AI_Shoot()
{
	//PlaySound(ESound::TricksterShoot);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::SpyProjectile, false, NAME_None, true);
}
