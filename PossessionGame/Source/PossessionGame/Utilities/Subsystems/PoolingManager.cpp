#include "PoolingManager.h"
#include "DelegateManager.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Projectiles/Throwables/RangerMine.h"

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

TObjectPtr<APoolObject> UPoolingManager::SpawnRangerMineFromPool(const FVector& InLocation, const FRotator& InRotation)
{
	TObjectPtr<ARangerMine> RangerMine;
	if (RangerExistingMines.Num() >= RangerMineAmountCap)
	{
		RangerMine = RangerExistingMines[0];
		RangerExistingMines.RemoveAt(0);
		RangerMine->DestroyThis();
	}
	else
	{
		if (Pool[EPoolObject::RangerMine].Objects.IsEmpty()) AllocatePoolObject(EPoolObject::RangerMine);
		RangerMine = Cast<ARangerMine>(Pool[EPoolObject::RangerMine].Objects.Pop());
	}
	RangerMine->Init();
    RangerMine->SetActorLocationAndRotation(InLocation, InRotation);
	RangerExistingMines.Add(RangerMine);
	return RangerMine;
}

void UPoolingManager::Init(TMap<EPoolObject, TSubclassOf<APoolObject>>& InPoolMap)
{
	for (TTuple<EPoolObject, TSubclassOf<APoolObject>>& Element : InPoolMap)
	{
		Pool.Add(Element.Key);
		Pool[Element.Key].ObjectClass = Element.Value;
	}
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.BindUObject(this, &UPoolingManager::SpawnDamageNumber);
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestRangerMineRemoval.BindUObject(this, &UPoolingManager::RemoveRangerMineFromExistingMines);
}

void UPoolingManager::ReturnObjectToPool(const EPoolObject InObjectName, const TObjectPtr<APoolObject>& InPoolObject)
{
	InPoolObject->Disable();
	Pool[InObjectName].Objects.Add(InPoolObject);
}

void UPoolingManager::ReturnRangerMineToPool(const TObjectPtr<APoolObject>& InPoolObject)
{
	InPoolObject->Disable();
	Pool[EPoolObject::RangerMine].Objects.Add(InPoolObject);
}

void UPoolingManager::AllocatePoolObject(const EPoolObject InObjectName, const FVector& InLocation, const FRotator& InRotation)
{
	Pool[InObjectName].Objects.Push(GetWorld()->SpawnActor<APoolObject>(Pool[InObjectName].ObjectClass, InLocation, InRotation));
}

void UPoolingManager::SpawnDamageNumber(const FLinearColor InColor, const float InDamage, const FVector& InLocation, const FVector& InForwardActor)
{
	SpawnObjectFromPool(EPoolObject::DamageNumber, InLocation + InForwardActor * 25.f, FRotator::ZeroRotator)->Init(InDamage, InColor);
}
