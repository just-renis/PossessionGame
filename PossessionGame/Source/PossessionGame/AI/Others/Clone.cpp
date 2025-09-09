#include "Clone.h"
#include "CloneAnimationInstance.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

AClone::AClone()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = Mesh;
	FirstWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FirstWeaponMesh");
	SecondWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SecondWeaponMesh");
}

void AClone::InitClone(const bool bIsSpawnedByPlayer, const TObjectPtr<USkeletalMesh> InSkeletalMesh, const TObjectPtr<USkeletalMesh> InWeaponMesh)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->DespawnPlayerClones.AddUObject(this, &AClone::DestroyThis);
	if (!Mesh->GetSkeletalMeshAsset())
	{
		Mesh->SetSkeletalMeshAsset(InSkeletalMesh);
		Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
		Mesh->bEnableUpdateRateOptimizations = true;
		Mesh->SkipUpdateOverlapsOptimEnabled = true;
		Mesh->SetAllowAnimCurveEvaluation(false);
		Mesh->bUseRefPoseOnInitAnim = true;
		Mesh->SetComponentTickInterval(1.f / 30.f);
		AnimInstance = Cast<UCloneAnimationInstance>(Mesh->GetAnimInstance());
	}
	else Mesh->SetComponentTickEnabled(true);
	if (!FirstWeaponMesh->GetSkeletalMeshAsset() || !SecondWeaponMesh->GetSkeletalMeshAsset())
	{
		FirstWeaponMesh->SetSkeletalMeshAsset(InWeaponMesh);
		SecondWeaponMesh->SetSkeletalMeshAsset(InWeaponMesh);
		FirstWeaponMesh->SetComponentTickEnabled(false);
		SecondWeaponMesh->SetComponentTickEnabled(false);
		FirstWeaponMesh->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AIWeaponSocket1"));
		SecondWeaponMesh->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AIWeaponSocket2"));
	}
	AnimInstance->SetIsEnabled(true);
	GetWorldTimerManager().SetTimer(AnimBlendSpaceMovementSpeedHandle, this, &AClone::SetAnimBlendSpaceMovementSpeed, 0.05f, true);
	Super::InitClone(bIsSpawnedByPlayer, InSkeletalMesh, InWeaponMesh);
}

void AClone::DestroyThis()
{
	AnimInstance->SetIsEnabled(false);
	Mesh->SetComponentTickEnabled(false);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->DespawnPlayerClones.RemoveAll(this);
	GetWorldTimerManager().ClearTimer(AnimBlendSpaceMovementSpeedHandle);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::Clone, this);
}

void AClone::SetAnimBlendSpaceMovementSpeed() const
{
	AnimInstance->SetSpeed(100.f);
}
