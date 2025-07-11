#include "GuardianSingularWeapon.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void AGuardianSingularWeapon::CancelShootingHold()
{
	bIsShootingHolding = false;
	ShootingHoldDuration = 0.f;
}

void AGuardianSingularWeapon::ToggleMeshAbilityState(const bool bState) const
{
	//if (!MID_GuardianWeaponMaterial) return;
//	MID_GuardianWeaponMaterial->SetScalarParameterValue("Hue", bState ? 0.9f : 1.f);
	//MID_GuardianWeaponMaterial->SetScalarParameterValue("Emissive Effect", bState ? 1.f : 0.f);
//	MID_GuardianWeaponMaterial->SetScalarParameterValue("Width", bState ? 0.2f : 0.f);
}

AGuardianSingularWeapon::AGuardianSingularWeapon()
{
	Sounds.Add(ESound::GuardianShoot, CreateDefaultSubobject<UAudioComponent>(TEXT("shootSound")));
	Sounds[ESound::GuardianShoot]->SetAutoActivate(false);
	Sounds[ESound::GuardianShoot]->SetupAttachment(GetMesh());
}

void AGuardianSingularWeapon::AI_Shoot()
{
	PlaySound(ESound::GuardianShoot, true);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::GuardianProjectile);
}

float AGuardianSingularWeapon::P_ReceiveHitScanDamage()
{
	const int32 NumIntervals = FMath::Clamp(FMath::FloorToInt(ShootingHoldDuration * 2.f), 0, 10);
	return HitScanDamage + DefaultHitScanDamage * 0.1f * NumIntervals;
}

void AGuardianSingularWeapon::P_Shoot(const bool bIsFirstWeapon)
{
	if (ShootTimer >= ShootCooldown)
	{
		AssetsManager->PlaySound2D(ESound::GuardianShoot);
		MuzzleFlashLogic();
		MakeShootingCameraShake();
		ShootTimer = 0.f;
		bIsHitScanEnabled ? HitScanLogic() : ShootProjectile(EPoolObject::GuardianProjectile);
	}
}

void AGuardianSingularWeapon::BeginPlay()
{
	Name = EWeapon::GuardianSingularWeapon;
	Super::BeginPlay();
}

void AGuardianSingularWeapon::SetMeshes(EWeapon PrimaryDataAssetName)
{
	if (!Mesh->GetSkeletalMeshAsset()) Mesh->SetSkeletalMesh(GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->GetWeaponData(PrimaryDataAssetName)->WeaponMesh.Get());
	Mesh->SetMaterial(0, MI_GuardianWeaponMaterial);
	//MID_GuardianWeaponMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
	//Mesh->SetMaterial(0, MID_GuardianWeaponMaterial);
	OriginalScale = Mesh->GetRelativeScale3D();
}

void AGuardianSingularWeapon::Tick(const float InDeltaSeconds)
{
	Super::Tick(InDeltaSeconds);
	if (bIsShootingHolding) ShootingHoldDuration += InDeltaSeconds;
}
