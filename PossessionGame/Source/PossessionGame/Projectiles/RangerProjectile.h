#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "RangerProjectile.generated.h"

UCLASS()
class POSSESSIONGAME_API ARangerProjectile : public AProjectile
{
	GENERATED_BODY()

	virtual float AI_UpgradeDamageAppliance() override;
};
