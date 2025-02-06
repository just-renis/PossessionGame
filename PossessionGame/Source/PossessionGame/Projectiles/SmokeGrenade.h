#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "SmokeGrenade.generated.h"

UCLASS()
class POSSESSIONGAME_API ASmokeGrenade : public AProjectile
{
	GENERATED_BODY()
public:
	ASmokeGrenade();
private:
	virtual void LoadSounds() override;
	virtual void PlayImpactSound() override;
};
