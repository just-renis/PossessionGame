#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolingManager.generated.h"

enum class EPoolObject : uint8;
class APoolObject;

USTRUCT()
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
	TObjectPtr<APoolObject> SpawnObjectFromPool(const EPoolObject InObjectName, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator);
	
	void Init(TMap<EPoolObject, TSubclassOf<APoolObject>>& InPoolMap);
	void ReturnObjectToPool(const EPoolObject InObjectName, const TObjectPtr<APoolObject>& InPoolObject);
private:
	UPROPERTY()
	TMap<EPoolObject, FObjectPoolData> Pool;

	void AllocatePoolObject(const EPoolObject InObjectName, const FVector& InLocation, const FRotator& InRotation);
};
