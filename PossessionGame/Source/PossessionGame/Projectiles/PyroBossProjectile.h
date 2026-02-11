#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "PyroBossProjectile.generated.h"

UCLASS()
class POSSESSIONGAME_API APyroBossProjectile : public AProjectile
{
	GENERATED_BODY()

	APyroBossProjectile();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDamageOverTimeData> Data;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ImpactSfx;

	virtual void OnImpact(const FHitResult& Hit) override;
	virtual void PlayImpactSound() override;
};
