#include "PlayerWeaponData.h"

TObjectPtr<UMaterialInterface> UPlayerWeaponData::GetVariationMaterial(const EVariation InVariation)
{
	return VariationMaterials.Contains(InVariation) ? VariationMaterials[InVariation] : nullptr;
}
