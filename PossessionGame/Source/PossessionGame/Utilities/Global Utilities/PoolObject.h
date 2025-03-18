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
	virtual void Init(bool bInIsPlayerObject);
	virtual void Init(const float InData);
	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale, const int32 InWhich = 0); // For projectile pools, since there is no other way to setup projectile efficiently.
	virtual void Disable();
	void SetPoolObjectState(const bool bState);
};
