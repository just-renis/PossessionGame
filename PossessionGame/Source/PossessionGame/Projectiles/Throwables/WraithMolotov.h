#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "WraithMolotov.generated.h"

UCLASS()
class POSSESSIONGAME_API AWraithMolotov : public AThrowable
{
	GENERATED_BODY()

	AWraithMolotov();
	
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	float DefaultRadius = 12.f;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeploySfx;

	virtual void OnImpact(const FHitResult& InHit) override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
