#include "Clone.h"

AClone::AClone()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = Mesh;
}

void AClone::InitClone(const bool bIsSpawnedByPlayer, const TObjectPtr<USkeletalMesh> InSkeletalMesh)
{
	Super::InitClone(bIsSpawnedByPlayer, InSkeletalMesh);
	Mesh->SetSkeletalMeshAsset(InSkeletalMesh);
	Mesh->PlayAnimation(WalkAnim, true);
	Mesh->GetAnimInstance()->Montage_Play(ShootMontage);
}
