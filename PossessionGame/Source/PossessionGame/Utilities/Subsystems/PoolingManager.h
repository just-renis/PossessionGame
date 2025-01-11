#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/PoolObject.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolingManager.generated.h"

class UPoolObjectData;
struct FProjectileData;
enum class EPoolObject : uint8;
class APoolObject;

USTRUCT(BlueprintType)
struct POSSESSIONGAME_API FObjectPoolData
{
	GENERATED_BODY()

	TSubclassOf<APoolObject> ObjectClass;
	TArray<TObjectPtr<APoolObject>> Objects;
};

UCLASS()
class POSSESSIONGAME_API UPoolingManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	TObjectPtr<APoolObject> SpawnObjectFromPool(bool bIsPlayerObject, EPoolObject ObjectName, const FVector& Location, const FRotator& Rotation);
	
	void Init(TMap<EPoolObject, TSubclassOf<APoolObject>>& Map);
	void ReturnObjectToPool(EPoolObject ObjectName, const TObjectPtr<APoolObject>& PoolObject);
	void SpawnObjectFromPool(const FProjectileData& ProjectileData, const FVector& Location, const FRotator& Rotation, const FVector& Scale); // For projectile pools, since there is no other way to setup projectile efficiently.
private:
	UPROPERTY()
	TMap<EPoolObject, FObjectPoolData> Pool;

	void AllocatePoolObject(EPoolObject ObjectName);
	void SetPoolObjectState(const TObjectPtr<APoolObject>& PoolObject, const bool bState);
};
