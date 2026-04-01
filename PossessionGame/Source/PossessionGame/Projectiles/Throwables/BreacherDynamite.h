#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Interfaces/DamageReasonProvider.h"
#include "BreacherDynamite.generated.h"

UCLASS()
class POSSESSIONGAME_API ABreacherDynamite : public AThrowable, public IDamageReasonProvider
{
	GENERATED_BODY()
public:
	virtual EDamageReason GetDamageReason() const override { return EDamageReason::BreacherDynamiteDamage; }
private:
	ABreacherDynamite();

	float SpecialExplosionDelay = 0.3f;
	FTimerHandle SpecialExplosionDelayHandle;
	FVector SavedSpecialExplosionLocation;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ExplodeSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> StickSfx;
	
	void PrimaryExplode();
	void Explode();
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
