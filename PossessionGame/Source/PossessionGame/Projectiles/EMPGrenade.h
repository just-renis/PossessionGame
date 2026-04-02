#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "EMPGrenade.generated.h"

class USphereComponent;

UCLASS()
class POSSESSIONGAME_API AEMPGrenade : public AProjectile
{
	GENERATED_BODY()

	AEMPGrenade();

	float EffectDuration;
	UPROPERTY(EditDefaultsOnly, Category = "Radius", meta = (AllowPrivateAccess = true))
	float EmpRadius = 250.f;

	virtual void PlayImpactSound() override;
	virtual void OnImpact(const FHitResult& Hit) override;
	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich) override;
};
