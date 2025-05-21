#include "GuardianSingularWeapon.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void AGuardianSingularWeapon::CancelShootingHold()
{
	bIsShootingHolding = false;
	ShootingHoldDuration = 0.f;
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
	const int32 NumIntervals = FMath::Clamp(FMath::FloorToInt(ShootingHoldDuration / 0.5f), 0, 10);
	return HitScanDamage + DefaultHitScanDamage * 0.1f * NumIntervals;
}

void AGuardianSingularWeapon::P_Shoot(const bool bIsFirstWeapon)
{
	if (ShootTimer >= ShootCooldown)
	{
		PlaySound(ESound::GuardianShoot, true);
		MuzzleFlashLogic();
		MakeShootingCameraShake();
		ShootTimer = 0.f;
		bIsHitScanEnabled ? HitScanLogic() : ShootProjectile(EPoolObject::GuardianProjectile);
	}
}

void AGuardianSingularWeapon::P_ApplyCurrentVariation()
{
	if (EVariation::GuardianVariationGold == Proprietor->GetGameInstance()->GetSaveSlot()->GetSelectedVariation(Proprietor->GetCharacterName()))
	{
		HitScanDamage = DefaultHitScanDamage *= 0.7f;
	}
}

void AGuardianSingularWeapon::BeginPlay()
{
	Name = EWeapon::GuardianSingularWeapon;
	Super::BeginPlay();
}

void AGuardianSingularWeapon::SetMeshes(EWeapon PrimaryDataAssetName)
{
	if (!Mesh->GetSkeletalMeshAsset()) Mesh->SetSkeletalMesh(GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->GetWeaponData(PrimaryDataAssetName)->WeaponMesh.Get());
	Mesh->SetMaterial(0, MI_GuardianWeaponMaterial);
	OriginalScale = Mesh->GetRelativeScale3D();
}

void AGuardianSingularWeapon::Tick(const float InDeltaSeconds)
{
	Super::Tick(InDeltaSeconds);
	if (bIsShootingHolding) ShootingHoldDuration += InDeltaSeconds;
}
