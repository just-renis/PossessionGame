#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "GravitationalPull.generated.h"

UCLASS()
class POSSESSIONGAME_API AGravitationalPull : public AThrowable
{
	GENERATED_BODY()

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void OnImpact(const FHitResult& InHit) override;
};
