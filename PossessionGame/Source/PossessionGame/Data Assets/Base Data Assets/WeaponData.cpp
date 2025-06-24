#include "WeaponData.h"
#include "PossessionGame/Enums/EPoolObject.h"

FProjectileData* UWeaponData::GetProjectileData(const EPoolObject InProjectileName)
{
	if (FProjectileData* FoundData = ProjectilesData.Find(InProjectileName))
	{
		if (FoundData->ProjectileName == EPoolObject::None) FoundData->ProjectileName = InProjectileName;
		return FoundData;
	}
	return nullptr;
}

void UWeaponData::SetAllProjectilesWalkSpread(bool bState)
{
	for (TPair<EPoolObject, FProjectileData> ProjectileData : ProjectilesData) ProjectileData.Value.SetWalkSpread(bState);
}
