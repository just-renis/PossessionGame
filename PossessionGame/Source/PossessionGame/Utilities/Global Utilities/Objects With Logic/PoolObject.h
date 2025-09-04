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
	virtual void Init(bool bInIsPlayerObject = true);
	virtual void Init(const float InData, const bool bHide = true);
	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich = 0); // For projectile pools, since there is no other way to setup projectile efficiently.
	virtual void Disable(const bool bHide = true);
	void SetPoolObjectState(const bool bState, const bool bHide = true);
};
