#include "PoolingManager.h"
#include "DelegateManager.h"
#include "SaveSlotManager.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Projectiles/Throwables/BreacherDynamite.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Projectiles/Throwables/RangerMine.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"

bool UPoolingManager::CanRangerThrowMine()
{
	RangerMineIndexToUse = -1;
	const int32 CapFromUpgrade = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::RangerAbility3, EUpgradeAttribute::Cap, true);
	RangerMineAmountCap = DefaultRangerMineAmountCap + CapFromUpgrade;
	if (RangerExistingMines.Num() < RangerMineAmountCap) return true;
	for (int32 i = 0; i < RangerExistingMines.Num(); i++)
	{
		if (!RangerExistingMines[i]->GetInExplosionProcess())
		{
			RangerMineIndexToUse = i;
			return true;
		}
	}
	return false;
}

bool UPoolingManager::CanBreacherThrowDynamite()
{
	BreacherDynamiteIndexToUse = -1;
	const int32 CapFromUpgrade = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::BreacherAbility3, EUpgradeAttribute::Cap, true);
	BreacherDynamiteAmountCap = DefaultBreacherDynamiteAmountCap + CapFromUpgrade;
	if (BreacherExistingDynamites.Num() < BreacherDynamiteAmountCap) return true;
	for (int32 i = 0; i < BreacherExistingDynamites.Num(); i++)
	{
		if (!BreacherExistingDynamites[i]->GetInExplosionProcess())
		{
			BreacherDynamiteIndexToUse = i;
			return true;
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

TObjectPtr<APoolObject> UPoolingManager::SpawnRangerMineFromPool(const FVector& InLocation, const FRotator& InRotation)
{
	TObjectPtr<APoolObject> RangerMine;
	if (RangerExistingMines.Num() < RangerMineAmountCap) RangerMineIndexToUse = -1;
	if (RangerMineIndexToUse != -1)
	{
		ReturnRangerMineToPool(RangerExistingMines[RangerMineIndexToUse]);
		RangerExistingMines[RangerMineIndexToUse]->Disable(true);
		RangerExistingMines.RemoveAt(RangerMineIndexToUse);
		RangerMine = Pool[EPoolObject::RangerMine].Objects.Pop();
	}
	if (!RangerMine)
	{
		if (Pool[EPoolObject::RangerMine].Objects.IsEmpty()) AllocatePoolObject(EPoolObject::RangerMine);
		RangerMine = Pool[EPoolObject::RangerMine].Objects.Pop();
	}
	RangerMine->Init();
    RangerMine->SetActorLocationAndRotation(InLocation, InRotation);
	RangerExistingMines.Add(Cast<ARangerMine>(RangerMine));
	return RangerMine;
}

TObjectPtr<APoolObject> UPoolingManager::SpawnBreacherDynamiteFromPool(const FVector& InLocation, const FRotator& InRotation)
{
	TObjectPtr<APoolObject> BreacherDynamite;
	if (BreacherExistingDynamites.Num() < BreacherDynamiteAmountCap) BreacherDynamiteIndexToUse = -1;
	if (BreacherDynamiteIndexToUse != -1)
	{
		ReturnBreacherDynamiteToPool(BreacherExistingDynamites[BreacherDynamiteIndexToUse]);
		BreacherExistingDynamites[BreacherDynamiteIndexToUse]->Disable(true);
		BreacherExistingDynamites.RemoveAt(BreacherDynamiteIndexToUse);
		BreacherDynamite = Pool[EPoolObject::BreacherDynamite].Objects.Pop();
	}
	if (!BreacherDynamite)
	{
		if (Pool[EPoolObject::BreacherDynamite].Objects.IsEmpty()) AllocatePoolObject(EPoolObject::BreacherDynamite);
		BreacherDynamite = Pool[EPoolObject::BreacherDynamite].Objects.Pop();
	}
	BreacherDynamite->Init();
	BreacherDynamite->SetActorLocationAndRotation(InLocation, InRotation);
	BreacherExistingDynamites.Add(Cast<ABreacherDynamite>(BreacherDynamite));
	return BreacherDynamite;
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

void UPoolingManager::ReturnBreacherDynamiteToPool(const TObjectPtr<APoolObject>& InPoolObject)
{
	InPoolObject->Disable();
	Pool[EPoolObject::BreacherDynamite].Objects.Add(InPoolObject);
}

void UPoolingManager::AllocatePoolObject(const EPoolObject InObjectName, const FVector& InLocation, const FRotator& InRotation)
{
	Pool[InObjectName].Objects.Push(GetWorld()->SpawnActor<APoolObject>(Pool[InObjectName].ObjectClass, InLocation, InRotation));
}

void UPoolingManager::SpawnDamageNumber(const FLinearColor InColor, const float InDamage, const FVector& InLocation, const FVector& InForwardActor)
{
	SpawnObjectFromPool(EPoolObject::DamageNumber, InLocation + InForwardActor * 25.f, FRotator::ZeroRotator)->Init(InDamage, InColor);
}
