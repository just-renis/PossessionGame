#include "PoolingManager.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/PoolObject.h"

TObjectPtr<APoolObject> UPoolingManager::SpawnObjectFromPool(bool bIsPlayerObject, EPoolObject ObjectName, const FVector& Location, const FRotator& Rotation)
{
	if (Pool[ObjectName].Objects.IsEmpty()) AllocatePoolObject(ObjectName);
	TObjectPtr<APoolObject> PoolObject = Pool[ObjectName].Objects.Pop();
	SetPoolObjectState(PoolObject, true);
	PoolObject->SetIsInUse(bIsPlayerObject, Location, Rotation);
	return PoolObject;
}

void UPoolingManager::Init(TMap<EPoolObject, TSubclassOf<APoolObject>>& Map)
{
	for (TTuple<EPoolObject, TSubclassOf<APoolObject>>& Element : Map)
	{
		Pool.Add(Element.Key);
		Pool[Element.Key].ObjectClass = Element.Value;
	}
}

void UPoolingManager::ReturnObjectToPool(EPoolObject ObjectName, TObjectPtr<APoolObject> PoolObject)
{
	SetPoolObjectState(PoolObject, false);
	Pool[ObjectName].Objects.Add(PoolObject);
}

void UPoolingManager::SpawnObjectFromPool(const FProjectileData& ProjectileData, const FVector& Location, const FRotator& Rotation, const FVector& Scale)
{
	if (Pool[ProjectileData.ProjectileName].Objects.IsEmpty()) AllocatePoolObject(ProjectileData.ProjectileName);
	const TObjectPtr<APoolObject> PoolObject = Pool[ProjectileData.ProjectileName].Objects.Pop();
	SetPoolObjectState(PoolObject, true);
	PoolObject->SetIsInUse(ProjectileData, Location, Rotation, Scale);
}

void UPoolingManager::AllocatePoolObject(EPoolObject ObjectName)
{
	Pool[ObjectName].Objects.Push(GetWorld()->SpawnActor<APoolObject>(Pool[ObjectName].ObjectClass, FVector::ZeroVector, FRotator::ZeroRotator));
}

void UPoolingManager::SetPoolObjectState(TObjectPtr<APoolObject> PoolObject, bool bState)
{
	PoolObject->SetActorEnableCollision(bState);
	PoolObject->SetActorHiddenInGame(!bState);
	PoolObject->SetActorTickEnabled(bState);
}
