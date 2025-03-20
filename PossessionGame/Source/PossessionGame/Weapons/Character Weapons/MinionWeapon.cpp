#include "MinionWeapon.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

void AMinionWeapon::BeginPlay()
{
	Name = EWeapon::MinionWeapon;
	Super::BeginPlay();
}

void AMinionWeapon::SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh)
{
	const TObjectPtr<UWeaponManager> WeaponManager = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>();
	FirstWeapon = WeaponManager->GetWeapon(GetWorld(), EWeapon::MinionSingularWeapon, CharacterMesh);
	SecondWeapon = WeaponManager->GetWeapon(GetWorld(), EWeapon::MinionSingularWeapon, CharacterMesh, FName("AIWeaponSocket2"));
}

void AMinionWeapon::AttachVfx(const EVfx InVfxName, const FName InSocketName)
{
	FirstWeapon->AttachVfx(InVfxName, InSocketName);
	SecondWeapon->AttachVfx(InVfxName, InSocketName);
}
