#include "TankShield.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"

void ATankShield::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	SetActorScale3D(FVector(3.f));
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
