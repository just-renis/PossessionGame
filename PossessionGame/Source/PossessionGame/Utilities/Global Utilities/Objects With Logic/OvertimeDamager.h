#pragma once

#include "CoreMinimal.h"
#include "PoolObject.h"
#include "OvertimeDamager.generated.h"

enum class EDamageReason : uint8;
enum class EAbility : uint8;
class AEnemy;
class USphereComponent;

UCLASS()
class POSSESSIONGAME_API AOvertimeDamager : public APoolObject
{
	GENERATED_BODY()
public:
	void Setup(const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason);
	virtual void Init(bool bInIsPlayerObject) override;
private:
	AOvertimeDamager();

	bool bIsPlayer;
	float Damage;
	EDamageReason DamageReason;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Radius;
	UPROPERTY()
	FTimerHandle DamageOvertimeHandle;
	UPROPERTY()
	FTimerHandle DestroyThisHandle;
	UPROPERTY()
	TArray<TObjectPtr<AEnemy>> Targets;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void DestroyThis();
	void DealOvertimeDamage();
	UFUNCTION()
	void AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void RemoveOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
