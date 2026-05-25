#include "TricksterSingularWeapon.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"

ATricksterSingularWeapon::ATricksterSingularWeapon()
{
	Sounds.Add(ESound::TricksterShoot, CreateDefaultSubobject<UAudioComponent>(TEXT("shootSound")));
	Sounds[ESound::TricksterShoot]->SetAutoActivate(false);
	Sounds[ESound::TricksterShoot]->SetupAttachment(GetMesh());
}

void ATricksterSingularWeapon::AI_Shoot()
{
	PlaySound(ESound::TricksterShoot, true);
	Super::AI_Shoot();
	ShootProjectile(EPoolObject::TricksterProjectile, false);
}

bool ATricksterSingularWeapon::P_CanReload(int32 OtherAmmoSource)
{
	return (OtherAmmoSource != -1 ? OtherAmmoSource > 0 : TotalAmmo > 0) && CurrentAmmo != MagazineCap;
}

void ATricksterSingularWeapon::P_Shoot(const EPoolObject InProjectileName)
{
	if (ShootTimer >= GetShootCooldown())
	{
		AssetsManager->PlaySound2D(ESound::TricksterShoot);
		MuzzleFlashLogic();
		MakeShootingCameraShake();
		ShootTimer = 0.f;
		bIsHitScanEnabled ? HitScanLogic() : ShootProjectile(InProjectileName, false);
	}
}

void ATricksterSingularWeapon::BeginPlay()
{
	Name = EWeapon::TricksterSingularWeapon;
	Super::BeginPlay();
}

void ATricksterSingularWeapon::SetMeshes(EWeapon PrimaryDataAssetName)
{
	if (!Mesh->GetSkeletalMeshAsset()) Mesh->SetSkeletalMesh(GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->GetWeaponData(PrimaryDataAssetName)->WeaponMesh.Get());
	OriginalScale = Mesh->GetRelativeScale3D();
}

void ATricksterSingularWeapon::FpsTrailVfxLogic()
{
	if (!FpsTrailVfx) AssetsManager->AttachVfx(AssetsManager->GetProjectileVfxName(Proprietor->GetCharacterName(), EVfxType::FpsTrail), [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, &FpsTrailVfx, GetMesh(), FName("FpsProjectileSocket"), Proprietor->GetVariation(), false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	else AssetsManager->ActivateWithVariation(EVfx::TankHandBall, nullptr, FpsTrailVfx, false, Proprietor->GetVariation());
}

void ATricksterSingularWeapon::CriticalHitLogic()
{
	if (EVariation::TricksterDefault == GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(ECharacterType::Trickster))
	{
		GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddTricksterDefaultMovementBoost();
		Proprietor->RefreshMovementSpeed();
	}
}
