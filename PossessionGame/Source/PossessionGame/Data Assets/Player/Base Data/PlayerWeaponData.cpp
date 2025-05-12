#include "PlayerWeaponData.h"

float UPlayerWeaponData::GetDamageMultiplierFromRange(const float InMeters) const { return DamageRangeEntries.GetDamageMultiplier(InMeters); }

TObjectPtr<UMaterialInterface> UPlayerWeaponData::GetVariationMaterial(const EVariation InVariation)
{
	return VariationMaterials.Contains(InVariation) ? VariationMaterials[InVariation] : nullptr;
}
