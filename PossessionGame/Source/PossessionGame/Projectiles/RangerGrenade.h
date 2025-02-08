#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "RangerGrenade.generated.h"

UCLASS()
class POSSESSIONGAME_API ARangerGrenade : public AProjectile
{
	GENERATED_BODY()

	ARangerGrenade();
	
	virtual void PlayImpactSound() override;
};
