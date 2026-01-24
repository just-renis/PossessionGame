#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Interfaces/DamageReasonProvider.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "OvertimeDamager.generated.h"

class UDamageOverTimeData;
enum class EPoolObject : uint8;
enum class EDamageReason : uint8;
class AEnemy;
class USphereComponent;

UCLASS()
class POSSESSIONGAME_API AOvertimeDamager : public APoolObject, public IDamageReasonProvider
{
	GENERATED_BODY()
public:
	AOvertimeDamager();
	
	virtual void Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason);
	virtual void Setup(const bool bInIsPlayerObject, const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason);
protected:
	bool bIsPlayer;
	float Damage;
	float DefaultDamage;
	float DamageTick;
	float DefaultDamageTick;
	float EffectTick;
	float EffectStrength;
	EPoolObject ObjectName;
	EDamageReason DamageReason;
	UPROPERTY()
	TArray<TObjectPtr<AEnemy>> Targets;

	TObjectPtr<USphereComponent> GetCollisionComponent() { return Radius; }
	
	virtual void AddTarget(const TObjectPtr<AEnemy>& InTarget);
	virtual void RemoveTarget(const TObjectPtr<AEnemy>& InTarget);
	void SetupRadiusAndTimers(const float InAreaSize, const float InLifeSpan);
private:
	FTimerHandle DamageOvertimeHandle;
	FTimerHandle DestroyThisHandle;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Radius;
	
	virtual EDamageReason GetDamageReason() const override { return DamageReason; }

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Disable(const bool bHide) override;
	virtual void DealOvertimeDamage() {}
	void ReturnToPool();
	UFUNCTION()
	void AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void RemoveOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
