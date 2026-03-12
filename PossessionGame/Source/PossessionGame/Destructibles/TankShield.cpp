#include "TankShield.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"

void ATankShield::Init(const bool bInIsPlayerObject)
{
	SetActorScale3D(FVector(3.f));
	Super::Init(bInIsPlayerObject);
}

void ATankShield::SetThisLifeSpan()
{
	LifeSpan = Data->GetLifeSpan();
	Super::SetThisLifeSpan();
}

void ATankShield::SetUpgradesApplication()
{
	DamageReduction = Data->GetDamageReduction();
}
