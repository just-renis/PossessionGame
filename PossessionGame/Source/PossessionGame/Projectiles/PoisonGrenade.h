#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "PoisonGrenade.generated.h"

UCLASS()
class POSSESSIONGAME_API APoisonGrenade : public AProjectile
{
	GENERATED_BODY()

	APoisonGrenade();

	float ModifiedOvertimeAreaLifeSpan;
	float ModifiedOvertimeAreaSize;
	float DefaultOvertimeAreaSize;

	virtual void P_SetVariationMaterial() override;

	virtual void PlayImpactSound() override;
	virtual void SpawnSecondProjectileImpactVfx(const FHitResult& Hit) override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void SpawnProjectileImpactVfx(const FHitResult& InHit, const bool bFirstImpact) override;
};
