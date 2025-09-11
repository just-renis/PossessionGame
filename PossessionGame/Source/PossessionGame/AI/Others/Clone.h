#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Clone.generated.h"

class UCapsuleComponent;
class UCloneAnimationInstance;

UCLASS()
class POSSESSIONGAME_API AClone : public APoolObject
{
	GENERATED_BODY()
	
	AClone();

	bool bFirstWeaponTurnToShoot = true;
	float MovementSpeed = 100.f;
	FVector Velocity;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> FirstWeaponMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> SecondWeaponMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCapsuleComponent> MainCollision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FProjectileData ProjectileData;
	UPROPERTY()
	FTimerHandle GravityHandle;
	UPROPERTY()
	FTimerHandle MoveHandle;
	UPROPERTY()
	FTimerHandle ShootHandle;
	UPROPERTY()
	TObjectPtr<UCloneAnimationInstance> AnimInstance;
	UPROPERTY()
	TObjectPtr<AActor> Target;

	virtual void InitClone(const bool bIsSpawnedByPlayer, const TObjectPtr<USkeletalMesh> InSkeletalMesh, const TObjectPtr<USkeletalMesh> InWeaponMesh) override;
	void DestroyThis();
	void GravityLogic();
	void SetupMeshOptimization() const;
	void SetupWeaponsMeshesOptimization() const;
	void MoveTowardsTargetLogic();
	void ShootProjectile();
	void ChangeTarget(const TObjectPtr<AActor>& InActorThatDied);
};
