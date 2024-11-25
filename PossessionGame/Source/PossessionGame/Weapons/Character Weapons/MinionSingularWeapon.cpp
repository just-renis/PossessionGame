#include "MinionSingularWeapon.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void AMinionSingularWeapon::BeginPlay()
{
	Name = EWeapon::MinionSingularWeapon;
	Super::BeginPlay();
}

void AMinionSingularWeapon::SetMeshes(EWeapon PrimaryDataAssetName)
{
	if (!Mesh->GetSkeletalMeshAsset()) Mesh->SetSkeletalMesh(GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->GetWeaponData(PrimaryDataAssetName)->WeaponMesh.Get());
	OriginalScale = Mesh->GetRelativeScale3D();
}
