#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "SmokeGrenade.generated.h"

UCLASS()
class POSSESSIONGAME_API ASmokeGrenade : public AProjectile
{
	GENERATED_BODY()
	
	ASmokeGrenade();
	
	virtual void PlayImpactSound() override;
};
