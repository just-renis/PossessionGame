#pragma once

#include "CoreMinimal.h"
#include "PoolObject.h"
#include "OvertimeDamager.generated.h"

class AEnemy;
class USphereComponent;

UCLASS()
class POSSESSIONGAME_API AOvertimeDamager : public APoolObject
{
	GENERATED_BODY()
public:
	void Setup(const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize);
private:
	AOvertimeDamager();
	
	float Damage;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Radius;
	UPROPERTY()
	TArray<TObjectPtr<AEnemy>> Targets;

	UFUNCTION()
	void DestroyThis();
	UFUNCTION()
	void DealOvertimeDamage();
	UFUNCTION()
	void AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void RemoveOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
