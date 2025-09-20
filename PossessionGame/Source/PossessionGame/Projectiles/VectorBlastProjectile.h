#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "VectorBlastProjectile.generated.h"

UCLASS()
class POSSESSIONGAME_API AVectorBlastProjectile : public AProjectile
{
	GENERATED_BODY()

	AVectorBlastProjectile();

	UPROPERTY()
	TObjectPtr<UAudioComponent> HitSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> BounceSfx;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
