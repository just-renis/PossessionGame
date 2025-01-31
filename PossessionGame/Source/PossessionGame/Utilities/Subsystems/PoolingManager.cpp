#include "PoolingManager.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/PoolObject.h"

TObjectPtr<APoolObject> UPoolingManager::SpawnObjectFromPool(const EPoolObject InObjectName, const FVector& InLocation, const FRotator& InRotation)
{
	TObjectPtr<APoolObject> PoolObject;
	if (Pool[InObjectName].Objects.IsEmpty())
	{
		AllocatePoolObject(InObjectName, InLocation, InRotation);
		PoolObject = Pool[InObjectName].Objects.Pop();
	}
	else
	{
		PoolObject = Pool[InObjectName].Objects.Pop();
		PoolObject->SetActorLocationAndRotation(InLocation, InRotation);
	}
	return PoolObject;
}

void UPoolingManager::Init(TMap<EPoolObject, TSubclassOf<APoolObject>>& InPoolMap)
{
	for (TTuple<EPoolObject, TSubclassOf<APoolObject>>& Element : InPoolMap)
	{
		Pool.Add(Element.Key);
		Pool[Element.Key].ObjectClass = Element.Value;
	}
}

void UPoolingManager::ReturnObjectToPool(const EPoolObject InObjectName, const TObjectPtr<APoolObject>& InPoolObject)
{
	InPoolObject->Disable();
	Pool[InObjectName].Objects.Add(InPoolObject);
}

void UPoolingManager::AllocatePoolObject(const EPoolObject InObjectName, const FVector& InLocation, const FRotator& InRotation)
{
	Pool[InObjectName].Objects.Push(GetWorld()->SpawnActor<APoolObject>(Pool[InObjectName].ObjectClass, InLocation, InRotation));
}
