#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Clone.generated.h"

class UCapsuleComponent;
class UCloneAnimationInstance;

UCLASS()
class POSSESSIONGAME_API AClone : public APoolObject
{
	GENERATED_BODY()
	
	AClone();
	
	FVector Velocity;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> FirstWeaponMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> SecondWeaponMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCapsuleComponent> MainCollision;
	UPROPERTY()
	FTimerHandle AnimBlendSpaceMovementSpeedHandle;
	UPROPERTY()
	FTimerHandle GravityHandle;
	UPROPERTY()
	TObjectPtr<UCloneAnimationInstance> AnimInstance;

	virtual void InitClone(const bool bIsSpawnedByPlayer, const TObjectPtr<USkeletalMesh> InSkeletalMesh, const TObjectPtr<USkeletalMesh> InWeaponMesh) override;
	void DestroyThis();
	void SetAnimBlendSpaceMovementSpeed() const;
	void GravityLogic();
};
