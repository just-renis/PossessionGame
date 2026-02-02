#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "PyroBossProjectile.generated.h"

UCLASS()
class POSSESSIONGAME_API APyroBossProjectile : public AProjectile
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDamageOverTimeData> Data;

	virtual void OnImpact(const FHitResult& Hit) override;
};
