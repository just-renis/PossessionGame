#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "SpyProjectile.generated.h"

UCLASS()
class POSSESSIONGAME_API ASpyProjectile : public AProjectile
{
	GENERATED_BODY()

	virtual float AI_UpgradeDamageAppliance() override;
};
