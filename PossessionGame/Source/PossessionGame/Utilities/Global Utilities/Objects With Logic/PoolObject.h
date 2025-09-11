#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolObject.generated.h"

struct FProjectileData;

UCLASS()
class POSSESSIONGAME_API APoolObject : public AActor
{
	GENERATED_BODY()
public:
	virtual void Init(const bool bInIsPlayerObject = true);
	virtual void Init(const float InData, FLinearColor InColor, const bool bHide = true); // Damage numbers.
	virtual void InitClone(const bool bIsSpawnedByPlayer, const TObjectPtr<USkeletalMesh> InSkeletalMesh, const TObjectPtr<USkeletalMesh> InWeaponMesh); // Clones.
	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich = 0); // For projectile pools, since there is no other way to setup projectile efficiently.
	virtual void InitCloneProjectile(FProjectileData& InProjectileData, const FRotator& Rotation); // For clone projectile.
	virtual void Disable(const bool bHide = true);
	void SetPoolObjectState(const bool bState, const bool bHide = true);
};
