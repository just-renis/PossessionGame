#include "TankShield.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"

void ATankShield::SetThisLifeSpan()
{
	LifeSpan = Data->GetLifeSpan();
	Super::SetThisLifeSpan();
}

void ATankShield::SetUpgradesApplication()
{
	DamageReduction = Data->GetDamageReduction();
}
