#include "GuardianSingularWeapon.h"

#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

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
	ShootProjectile(EPoolObject::GuardianProjectile, false);
}

void AGuardianSingularWeapon::P_Shoot()
{
	if (ShootTimer >= GetShootCooldown())
	{
		AssetsManager->PlaySound2D(ESound::GuardianShoot);
		MuzzleFlashLogic();
		MakeShootingCameraShake();
		ShootTimer = 0.f;
		bIsHitScanEnabled ? HitScanLogic() : ShootProjectile(EPoolObject::GuardianProjectile, false);
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
	OriginalScale = Mesh->GetRelativeScale3D();
}

void AGuardianSingularWeapon::MuzzleLogicFromAbility()
{
	if (!VectorBlastMuzzleComponent)
	{
		AssetsManager->AttachVfx(EVfx::VectorBlastMuzzle, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); },
		&VectorBlastMuzzleComponent, Mesh, FName("MuzzleSocket"));
	}
	else VectorBlastMuzzleComponent->Activate(true);
}
