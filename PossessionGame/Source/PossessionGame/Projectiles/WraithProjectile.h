#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "WraithProjectile.generated.h"

UCLASS()
class POSSESSIONGAME_API AWraithProjectile : public AProjectile
{
	GENERATED_BODY()

	AWraithProjectile();

	FTimerHandle ExplosionVfxHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float ExplosionLength = 0.4f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ExplosionVfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ExplosionSfx;

	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich) override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void DestroyThis() override;
	void ReturnObjectToPoolAfterExplosionVfxFinished();
	virtual void DealAreaDamage() override;
};
