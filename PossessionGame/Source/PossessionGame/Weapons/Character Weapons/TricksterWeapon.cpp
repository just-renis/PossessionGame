#include "TricksterWeapon.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void ATricksterWeapon::P_Reload(int32 OtherAmmoSource)
{
	HandsAnimInstance->PlayMontage(EHandsAnimState::Reload);
	Super::P_Reload(OtherAmmoSource);
}

void ATricksterWeapon::P_SetShootMontage(bool bFirstWeapon, bool bState)
{
	if (bFirstWeapon) ShootMontageHandler.bIsFirstWeaponShooting = bState;
	else ShootMontageHandler.bIsSecondWeaponShooting = bState;
}

void ATricksterWeapon::P_SetupOnShootState()
{
	Cast<ATrickster>(Proprietor)->OnWeaponShootStateChange.BindUObject(this, &ATricksterWeapon::P_SetShootMontage);
}

void ATricksterWeapon::BeginPlay()
{
	Name = EWeapon::TricksterWeapon;
	Super::BeginPlay();
	SetupHandsAnimInstance();
}

void ATricksterWeapon::SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh)
{
	const TObjectPtr<UWeaponManager> WeaponManager = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>();
	FirstWeapon = WeaponManager->GetWeapon(GetWorld(), EWeapon::TricksterSingularWeapon, CharacterMesh);
	SecondWeapon = WeaponManager->GetWeapon(GetWorld(), EWeapon::TricksterSingularWeapon, CharacterMesh, FName("AIWeaponSocket2"));
}

void ATricksterWeapon::SetMeshes(EWeapon PrimaryDataAssetName)
{
	if (!HandsMesh->GetSkeletalMeshAsset()) HandsMesh->SetSkeletalMesh(GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->GetWeaponData(PrimaryDataAssetName)->HandsMesh.Get());
}

void ATricksterWeapon::PlayShootMontage()
{
	if (ShootMontageHandler.bIsFirstWeaponShooting && ShootMontageHandler.bIsSecondWeaponShooting) HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	else if (ShootMontageHandler.bIsFirstWeaponShooting) HandsAnimInstance->PlayMontage(EHandsAnimState::ShootLeft);
	else HandsAnimInstance->PlayMontage(EHandsAnimState::ShootRight);
}

void ATricksterWeapon::AttachMuzzleFlash(const EVfx InVfxName)
{
	FirstWeapon->AttachMuzzleFlash(InVfxName);
	SecondWeapon->AttachMuzzleFlash(InVfxName);
}
