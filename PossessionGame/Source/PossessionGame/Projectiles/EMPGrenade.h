#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "EMPGrenade.generated.h"

UCLASS()
class POSSESSIONGAME_API AEMPGrenade : public AProjectile
{
	GENERATED_BODY()

	AEMPGrenade();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Radius;
	
	virtual void PlayImpactSound() override;
	virtual void OnImpact(const FHitResult& Hit) override;
	void TriggerEMPEffect() const;
};
