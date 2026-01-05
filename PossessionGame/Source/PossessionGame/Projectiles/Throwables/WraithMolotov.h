#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "WraithMolotov.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AWraithMolotov : public AThrowable
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	float DefaultRadius = 12.f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> RadiusVfx;

	virtual void Disable(const bool bHide) override;
	virtual void OnImpact(const FHitResult& Hit) override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
