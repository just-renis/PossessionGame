#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "EMPGrenade.generated.h"

UCLASS()
class POSSESSIONGAME_API AEMPGrenade : public AProjectile
{
	GENERATED_BODY()

	AEMPGrenade();
	
	virtual void PlayImpactSound() override;
};
