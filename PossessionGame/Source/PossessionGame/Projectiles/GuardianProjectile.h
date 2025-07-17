#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "GuardianProjectile.generated.h"

UCLASS()
class POSSESSIONGAME_API AGuardianProjectile : public AProjectile
{
	GENERATED_BODY()

	virtual void P_VariationDamageAppliance() override;
};
