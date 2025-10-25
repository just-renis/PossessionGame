#include "BreacherWeapon.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ABreacherWeapon::P_Shoot()
{
	AssetsManager->PlaySound2D(ESound::BreacherShoot);
	HandsAnimInstance->PlayMontage(EHandsAnimState::Shoot);
	if (AnimInstance) AnimInstance->PlayMontage(EWeaponAnimState::Shoot);
	MuzzleFlashLogic();
	MakeShootingCameraShake();
	CurrentAmmo--;
	ShootTimer = 0.f;
	ShootProjectile(EPoolObject::BreacherProjectile, false, NAME_None, true);
	P_UpdateAmmoTextHUD();
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

void ABreacherWeapon::ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, bool bFromFirstWeapon)
{
	FProjectileData* ProjectileData = GetProjectileData(InProjectileName);
	if (!ProjectileData) return;
	ProjectileData->SetProprietor(Proprietor);
	const FName& ProjectileSocketName = InProjectileSocketName == NAME_None ? ProjectileData->ProjectileSocketName : InProjectileSocketName;
	const FVector BaseLocation = GetSocketLocation(ProjectileSocketName);
	const FRotator BaseRotation = GetSocketRotation(ProjectileSocketName);
	for (int32 i = 0; i < 8; i++)
	{
		const float YawOffset = FMath::RandRange(-ProjectileData->GetCalcSpread(), ProjectileData->GetCalcSpread());
		const float PitchOffset = FMath::RandRange(-ProjectileData->GetCalcSpread(), ProjectileData->GetCalcSpread());
		const FRotator SpreadRotation = (BaseRotation + FRotator(PitchOffset, YawOffset, 0.f)).GetNormalized();
		GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(InProjectileName, BaseLocation, SpreadRotation)->Init(*ProjectileData, BaseRotation);
	}
}
