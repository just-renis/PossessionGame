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

	virtual float GetModifiedLifeSpan() const override;
	virtual float GetModifiedProjectileInitialSpeed(const float InBaseInitialSpeed) const override;
	virtual float GetModifiedProjectileMaxSpeed(const float InBaseMaxSpeed) const override;

	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich) override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
