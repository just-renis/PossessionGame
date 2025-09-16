#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "RangerMine.generated.h"

class USphereComponent;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ARangerMine : public AThrowable
{
	GENERATED_BODY()
	
	ARangerMine();

	bool bAlreadyExploded = false;
	bool bAlreadyLanded = false;
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> MineTriggerCollision;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> MineRadiusVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> MineImpactVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> MineExplosionVfx;

	virtual void BeginPlay() override;
	virtual void OnImpact(const FHitResult& Hit) override;
	void DealAreaDamage();
	UFUNCTION()
	void TriggeredCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
