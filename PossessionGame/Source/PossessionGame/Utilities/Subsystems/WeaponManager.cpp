#include "WeaponManager.h"
#include "PossessionGame/Weapons/Categories/DualWieldingWeapon.h"

TObjectPtr<AWeapon> UWeaponManager::GetWeapon(TObjectPtr<UWorld> World, EWeapon WeaponName, TObjectPtr<USkeletalMeshComponent> Mesh, FName SocketName)
{
    TObjectPtr<AWeapon> Weapon = World->SpawnActor<AWeapon>(Weapons[WeaponName], FVector::ZeroVector, FRotator::ZeroRotator);
	if (WeaponName == EWeapon::TricksterWeapon || WeaponName == EWeapon::MinionWeapon || WeaponName == EWeapon::GuardianWeapon) Cast<ADualWieldingWeapon>(Weapon)->SetupOneHandedWeapons(Mesh);
	Weapon->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	return Weapon;
}

void UWeaponManager::AddWeapon(EWeapon WeaponType, TSubclassOf<AWeapon> Weapon) { Weapons.Add(WeaponType, Weapon); }
