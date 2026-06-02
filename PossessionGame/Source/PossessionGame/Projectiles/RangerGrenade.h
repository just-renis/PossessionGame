#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "RangerGrenade.generated.h"

UCLASS()
class POSSESSIONGAME_API ARangerGrenade : public AProjectile
{
	GENERATED_BODY()

	ARangerGrenade();

	virtual float AI_UpgradeDamageAppliance() override;

	virtual float P_UpgradeDamageAppliance() override;

	virtual void P_SetVariationMaterial() override;
	
	virtual void PlayImpactSound() override;
};
