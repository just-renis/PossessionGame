#include "GuardianProjectile.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Weapons/Character Weapons/GuardianWeapon.h"

void AGuardianProjectile::VariationDamageAppliance()
{
	if (EVariation::GuardianVariationGold != GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(ProjectileData.GetProprietor()->GetCharacterName())) return;
	const int32 NumIntervals = FMath::Clamp(FMath::FloorToInt(Cast<AGuardianWeapon>(ProjectileData.GetProprietor()->GetWeapon())->P_GetShootingHoldDuration() * 2.f), 0, 10);
	Damage += DefaultDamage * 0.05f * NumIntervals;
}
