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
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
