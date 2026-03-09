#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "GravitationalPull.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AGravitationalPull : public AThrowable
{
	GENERATED_BODY()
public:
	virtual void InitWithProprietor(const TObjectPtr<AEnemy> InProprietor) override;
private:
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BallVfx;

	virtual void BeginPlay() override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void OnImpact(const FHitResult& InHit) override;
};
