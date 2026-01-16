#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "WraithTrap.generated.h"

UCLASS()
class POSSESSIONGAME_API AWraithTrap : public AThrowable
{
	GENERATED_BODY()
	
	virtual void OnImpact(const FHitResult& InHit) override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
