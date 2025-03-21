#include "MinionWeapon.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
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

void AMinionWeapon::PostProprietorSetup()
{
	AssetsManager->AttachOrLoadVfx(AssetsManager->GetVfxName(Proprietor->GetCharacterName(), EVfxType::Muzzle), &MuzzleFlashComponent, FirstWeapon->GetMesh(), FName("MuzzleSocket"), true);
	AssetsManager->AttachOrLoadVfx(AssetsManager->GetVfxName(Proprietor->GetCharacterName(), EVfxType::Muzzle), &MuzzleFlashComponent, SecondWeapon->GetMesh(), FName("MuzzleSocket"), true);
}
