#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "SamuraiShuriken.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ASamuraiShuriken : public AThrowable
{
	GENERATED_BODY()
public:
	virtual void InitWithProprietor(const TObjectPtr<AEnemy> InProprietor) override;
private:
	ASamuraiShuriken();
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> ImpactSfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ItselfVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ImpactVfx;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void OnImpact(const FHitResult& InHit) override;
	virtual void Disable(const bool bHide) override;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
