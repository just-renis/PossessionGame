#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Interfaces/DamageReasonProvider.h"
#include "OvertimeDamager.generated.h"

enum class EDamageReason : uint8;
class AEnemy;
class USphereComponent;

UCLASS()
class POSSESSIONGAME_API AOvertimeDamager : public AActor, public IDamageReasonProvider
{
	GENERATED_BODY()
public:
	AOvertimeDamager();
	
	virtual void Setup(const bool bInIsPlayerObject, const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason);
protected:
	bool bIsPlayer;
	float Damage;
	float DamageTick;
	EDamageReason DamageReason;
	UPROPERTY()
	TArray<TObjectPtr<AEnemy>> Targets;
	
	virtual void AddTarget(const TObjectPtr<AEnemy>& InTarget);
	virtual void RemoveTarget(const TObjectPtr<AEnemy>& InTarget);
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Radius;
	UPROPERTY()
	FTimerHandle DamageOvertimeHandle;
	UPROPERTY()
	FTimerHandle DestroyThisHandle;
	
	virtual EDamageReason GetDamageReason() const override { return DamageReason; }

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void DestroyThis();
	virtual void DealOvertimeDamage() {}
	UFUNCTION()
	void AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void RemoveOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
