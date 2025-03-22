#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "EMPGrenade.generated.h"

UCLASS()
class POSSESSIONGAME_API AEMPGrenade : public AProjectile
{
	GENERATED_BODY()

	AEMPGrenade();

	float EffectDuration;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Radius;
	
	virtual void PlayImpactSound() override;
	virtual void OnImpact(const FHitResult& Hit) override;
	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale, const int32 InWhich) override;
	virtual void Disable(const bool bHide) override;
	UFUNCTION()
	void TriggerEmpEffect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
