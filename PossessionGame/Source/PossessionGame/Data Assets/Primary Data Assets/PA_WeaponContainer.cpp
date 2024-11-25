#include "PA_WeaponContainer.h"

FPrimaryAssetId UPA_WeaponContainer::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Weapon"), WeaponName);
}