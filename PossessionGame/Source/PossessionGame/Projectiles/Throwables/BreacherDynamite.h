#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "BreacherDynamite.generated.h"

class UBoxComponent;

UCLASS()
class POSSESSIONGAME_API ABreacherDynamite : public AThrowable
{
	GENERATED_BODY()
public:
	virtual void InitWithProprietor(const TObjectPtr<AEnemy> InProprietor) override;
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
	virtual void OnImpact(const FHitResult& Hit) override;
};
