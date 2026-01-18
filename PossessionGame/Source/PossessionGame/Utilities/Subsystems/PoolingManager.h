#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolingManager.generated.h"

class UThrowableData;
class AThrowable;
enum class EPoolObject : uint8;
enum class EUpgrade : uint8;
class APoolObject;

USTRUCT()
struct POSSESSIONGAME_API FObjectPoolData
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<APoolObject> ObjectClass;
	UPROPERTY()
	TArray<TObjectPtr<APoolObject>> Objects;
};

USTRUCT()
struct POSSESSIONGAME_API FExistingObjectsWithCapData
{
	GENERATED_BODY()

	int32 IndexToUse = -1;
	int32 DefaultAmountCap = 1;
	int32 AmountCap = 1;
};

UCLASS()
class POSSESSIONGAME_API UPoolingManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	bool CanObjectBeThrown(const EPoolObject InObjectName);
	TObjectPtr<APoolObject> SpawnObjectFromPool(const EPoolObject InObjectName, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator);
	TObjectPtr<APoolObject> SpawnObjectWithCapFromPool(const EPoolObject InObjectName, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator);
	
	void Init(const TMap<EPoolObject, TSubclassOf<APoolObject>>& InPoolMap, const TMap<EPoolObject, TObjectPtr<UThrowableData>>& InThrowableData = {});
	void ReturnObjectToPool(const EPoolObject InObjectName, const TObjectPtr<APoolObject>& InPoolObject);
private:
	UPROPERTY()
	TMap<EPoolObject, FObjectPoolData> Pool;
	UPROPERTY()
	TMap<EPoolObject, FExistingObjectsWithCapData> ExistingObjectsWithCap;

	EUpgrade GetUpgradeFromPoolObjectName(const EPoolObject InObjectName);

	void AllocatePoolObject(const EPoolObject InObjectName, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotation = FRotator::ZeroRotator);
	void SpawnDamageNumber(const FLinearColor InColor, const float InDamage, const FVector& InLocation, const FVector& InForwardActor);
	void RemoveExistingObject(const EPoolObject InObjectName, const TObjectPtr<AThrowable>& InObject);
};
