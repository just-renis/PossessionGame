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
	virtual void SetIsInUse(bool bInIsPlayerObject, const FVector& Location, const FRotator& Rotation);
	virtual void SetIsInUse(FProjectileData& InProjectileData, const FVector& Location, const FRotator& Rotation, const FVector& Scale) {} // For projectile pools, since there is no other way to setup projectile efficiently.
};
