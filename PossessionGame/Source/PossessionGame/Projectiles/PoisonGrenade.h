#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "PoisonGrenade.generated.h"

UCLASS()
class POSSESSIONGAME_API APoisonGrenade : public AProjectile
{
	GENERATED_BODY()

	APoisonGrenade();
	
	virtual void PlayImpactSound() override;
};
