#include "PoolingManager.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/PoolObject.h"

TObjectPtr<APoolObject> UPoolingManager::SpawnObjectFromPool(const EPoolObject InObjectName)
{
	if (Pool[InObjectName].Objects.IsEmpty()) AllocatePoolObject(InObjectName);
	TObjectPtr<APoolObject> PoolObject = Pool[InObjectName].Objects.Pop();
	SetPoolObjectState(PoolObject, true);
	return PoolObject;
}

void UPoolingManager::Init(TMap<EPoolObject, TSubclassOf<APoolObject>>& PoolMap)
{
	for (TTuple<EPoolObject, TSubclassOf<APoolObject>>& Element : PoolMap)
	{
		Pool.Add(Element.Key);
		Pool[Element.Key].ObjectClass = Element.Value;
	}
}

void UPoolingManager::ReturnObjectToPool(EPoolObject ObjectName, const TObjectPtr<APoolObject>& PoolObject)
{
	SetPoolObjectState(PoolObject, false);
	Pool[ObjectName].Objects.Add(PoolObject);
}

void UPoolingManager::AllocatePoolObject(const EPoolObject InObjectName)
{
	Pool[InObjectName].Objects.Push(GetWorld()->SpawnActor<APoolObject>(Pool[InObjectName].ObjectClass, FVector::ZeroVector, FRotator::ZeroRotator));
}

void UPoolingManager::SetPoolObjectState(const TObjectPtr<APoolObject>& PoolObject, const bool bState)
{
	PoolObject->SetActorEnableCollision(bState);
	PoolObject->SetActorHiddenInGame(!bState);
	PoolObject->SetActorTickEnabled(bState);
}
