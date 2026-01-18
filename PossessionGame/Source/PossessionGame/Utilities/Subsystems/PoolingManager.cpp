#include "PoolingManager.h"
#include "DelegateManager.h"
#include "SaveSlotManager.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Projectiles/Throwables/RangerMine.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"

bool UPoolingManager::CanObjectBeThrown(const EPoolObject InObjectName)
{
	if (FExistingObjectsWithCapData* Data = ExistingObjectsWithCap.Find(InObjectName))
	{
		Data->IndexToUse = -1;
		const int32 CapFromUpgrade = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(GetUpgradeFromPoolObjectName(InObjectName), EUpgradeAttribute::Cap, true);
		Data->AmountCap = Data->DefaultAmountCap + CapFromUpgrade;
		if (Pool[InObjectName].Objects.Num() < Data->AmountCap) return true;
		for (int32 i = 0; i < Pool[InObjectName].Objects.Num(); i++)
		{
			if (!Pool[InObjectName].Objects[i]->GetIsActive())
			{
				Data->IndexToUse = i;
				return true;
			}
		}
		for (int32 i = 0; i < Pool[InObjectName].Objects.Num(); i++)
		{
			if (!Pool[InObjectName].Objects[i]->GetIsActionInProcess())
			{
				Data->IndexToUse = i;
				return true;
			}
		}
	}
	return false;
}

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

TObjectPtr<APoolObject> UPoolingManager::SpawnObjectWithCapFromPool(const EPoolObject InObjectName, const FVector& InLocation, const FRotator& InRotation)
{
	if (const FExistingObjectsWithCapData* Data = ExistingObjectsWithCap.Find(InObjectName))
	{
		TObjectPtr<APoolObject> Object;
		if (Data->IndexToUse != -1)
		{
			Object = Pool[InObjectName].Objects[Data->IndexToUse];
			Pool[InObjectName].Objects.RemoveAt(Data->IndexToUse);
			ReturnObjectToPool(InObjectName, Object);
			Object->Init();
			Object->SetActorLocationAndRotation(InLocation, InRotation);
		}
		if (!Object)
		{
			if (Pool[InObjectName].Objects.Num() < Data->AmountCap) AllocatePoolObject(InObjectName);
			Object = Cast<AThrowable>(Pool[InObjectName].Objects.Pop());
			Object->Init();
			Object->SetActorLocationAndRotation(InLocation, InRotation);
			Pool[InObjectName].Objects.Add(Object);
		}
		return Object;
	}
	return nullptr;
}

void UPoolingManager::Init(const TMap<EPoolObject, TSubclassOf<APoolObject>>& InPoolMap, const TMap<EPoolObject, TObjectPtr<UThrowableData>>& InThrowableData)
{
	for (const TPair<EPoolObject, TObjectPtr<UThrowableData>>& Element : InThrowableData)
	{
		ExistingObjectsWithCap.Add(Element.Key);
		ExistingObjectsWithCap[Element.Key].AmountCap = ExistingObjectsWithCap[Element.Key].DefaultAmountCap = Element.Value->GetCap();
	}
	for (const TPair<EPoolObject, TSubclassOf<APoolObject>>& Element : InPoolMap)
	{
		Pool.Add(Element.Key);
		Pool[Element.Key].ObjectClass = Element.Value;
	}
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.BindUObject(this, &UPoolingManager::SpawnDamageNumber);
	GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestObjectWithCapRemoval.BindUObject(this, &UPoolingManager::RemoveExistingObject);
}

void UPoolingManager::ReturnObjectToPool(const EPoolObject InObjectName, const TObjectPtr<APoolObject>& InPoolObject)
{
	InPoolObject->Disable();
	Pool[InObjectName].Objects.Add(InPoolObject);
}

EUpgrade UPoolingManager::GetUpgradeFromPoolObjectName(const EPoolObject InObjectName)
{
	switch (InObjectName)
	{
		case EPoolObject::RangerMine: return EUpgrade::RangerAbility3;
		case EPoolObject::BreacherBarrel: return EUpgrade::BreacherAbility1;
		case EPoolObject::BreacherDynamite: return EUpgrade::BreacherAbility3;
		default: return EUpgrade::None;
	}
}

void UPoolingManager::AllocatePoolObject(const EPoolObject InObjectName, const FVector& InLocation, const FRotator& InRotation)
{
	Pool[InObjectName].Objects.Push(GetWorld()->SpawnActor<APoolObject>(Pool[InObjectName].ObjectClass, InLocation, InRotation));
}

void UPoolingManager::SpawnDamageNumber(const FLinearColor InColor, const float InDamage, const FVector& InLocation, const FVector& InForwardActor)
{
	SpawnObjectFromPool(EPoolObject::DamageNumber, InLocation + InForwardActor * 25.f, FRotator::ZeroRotator)->Init(InDamage, InColor);
}

void UPoolingManager::RemoveExistingObject(const EPoolObject InObjectName, const TObjectPtr<AThrowable>& InObject)
{
	Pool[InObjectName].Objects.Remove(InObject);
}
