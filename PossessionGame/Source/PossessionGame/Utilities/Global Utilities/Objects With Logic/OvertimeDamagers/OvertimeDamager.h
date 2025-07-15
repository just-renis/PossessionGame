#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
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
	AOvertimeDamager();
	
	void Setup(const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason);
	virtual void Init(bool bInIsPlayerObject) override;
protected:
	float Damage;
	float DamageTick;
	EDamageReason DamageReason;
	UPROPERTY()
	TArray<TObjectPtr<AEnemy>> Targets;
private:
	bool bIsPlayer;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Radius;
	UPROPERTY()
	FTimerHandle DamageOvertimeHandle;
	UPROPERTY()
	FTimerHandle DestroyThisHandle;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void DestroyThis();
	virtual void DealOvertimeDamage() {}
	UFUNCTION()
	void AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void RemoveOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
