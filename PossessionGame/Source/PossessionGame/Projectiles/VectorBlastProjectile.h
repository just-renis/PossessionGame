#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "VectorBlastProjectile.generated.h"

UCLASS()
class POSSESSIONGAME_API AVectorBlastProjectile : public AProjectile
{
	GENERATED_BODY()

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
