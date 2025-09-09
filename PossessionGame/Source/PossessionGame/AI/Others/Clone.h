#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Clone.generated.h"

class UCloneAnimationInstance;

UCLASS()
class POSSESSIONGAME_API AClone : public APoolObject
{
	GENERATED_BODY()
	
	AClone();
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> FirstWeaponMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> SecondWeaponMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimationAsset> WalkAnim;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimMontage> ShootMontage;
	UPROPERTY()
	FTimerHandle AnimBlendSpaceMovementSpeedHandle;
	UPROPERTY()
	TObjectPtr<UCloneAnimationInstance> AnimInstance;

	virtual void InitClone(const bool bIsSpawnedByPlayer, const TObjectPtr<USkeletalMesh> InSkeletalMesh, const TObjectPtr<USkeletalMesh> InWeaponMesh) override;
	void DestroyThis();
	void SetAnimBlendSpaceMovementSpeed() const;
};
