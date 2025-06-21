#include "WeaponData.h"
#include "PossessionGame/Enums/EPoolObject.h"

FProjectileData& UWeaponData::GetProjectileData(EPoolObject ProjectileName)
{
	if (ProjectilesData[ProjectileName].ProjectileName == EPoolObject::None) ProjectilesData[ProjectileName].ProjectileName = ProjectileName;
	return ProjectilesData[ProjectileName];
}

void UWeaponData::SetAllProjectilesWalkSpread(bool bState)
{
	for (TPair<EPoolObject, FProjectileData> ProjectileData : ProjectilesData) ProjectileData.Value.SetWalkSpread(bState);
}
