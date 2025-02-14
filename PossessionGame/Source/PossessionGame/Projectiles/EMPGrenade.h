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
	virtual void Disable() override;
	UFUNCTION()
	void TriggerEMPEffect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
