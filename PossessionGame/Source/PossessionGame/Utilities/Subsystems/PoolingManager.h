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
	TObjectPtr<APoolObject> SpawnObjectFromPool(const EPoolObject InObjectName);
	
	void Init(TMap<EPoolObject, TSubclassOf<APoolObject>>& PoolMap);
	void ReturnObjectToPool(EPoolObject ObjectName, const TObjectPtr<APoolObject>& PoolObject);
private:
	UPROPERTY()
	TMap<EPoolObject, FObjectPoolData> Pool;

	void AllocatePoolObject(const EPoolObject InObjectName);
	void SetPoolObjectState(const TObjectPtr<APoolObject>& PoolObject, const bool bState);
};
