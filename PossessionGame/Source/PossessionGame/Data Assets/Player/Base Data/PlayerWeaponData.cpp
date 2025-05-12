#include "PlayerWeaponData.h"

float UPlayerWeaponData::GetDamageMultiplierFromRange(const float InMeters) const { return DamageRangeEntries.GetDamageMultiplier(InMeters); }

float UPlayerWeaponData::GetVariationHueValue(const EVariation InVariation)
{
	return VariationHueValues.Contains(InVariation) ? VariationHueValues[InVariation] : 0.f;
}
