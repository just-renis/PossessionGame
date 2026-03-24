#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "AcoBoss.generated.h"

class UAcoBossAnimationInstance;

UCLASS()
class POSSESSIONGAME_API AAcoBoss : public AEnemy
{
	GENERATED_BODY()
public:
	FVector GetCurrentTargetLocation() const;
	
	void SpawnFirstPhaseEnemy();
private:
	AAcoBoss();

	bool bIsFlyingTowardsHijackTarget = false;
	FVector DefaultSpawnLocation;
	FTimerHandle FlyingTowardsHijackEnemyHandle;
	TArray<ECharacterType> CharactersToHijack = { ECharacterType::Guardian, ECharacterType::Trickster, ECharacterType::Ranger, ECharacterType::Wraith,
		ECharacterType::Breacher, ECharacterType::Spy, ECharacterType::Tank, ECharacterType::Samurai };
	UPROPERTY(EditDefaultsOnly, Category = "First Phase", meta = (AllowPrivateAccess = true))
	float MovementSpeedTowardsHijackTarget = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "First Phase", meta = (AllowPrivateAccess = true))
	float StopDistance = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Default", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInstance> MI_Default;
	UPROPERTY()
	TObjectPtr<AEnemy> HijackTargetEnemy;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> FlyTrailVfx;
	UPROPERTY()
	TObjectPtr<UAcoBossAnimationInstance> AcoAnimInstance;
	
	virtual void AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection) override;

	virtual void BeginPlay() override;
	virtual void ApplyEmpEffect(const float InEffectDuration) override;
	virtual void ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent) override;
	virtual void InitiateDeath() override;
	void SetupFlyingTowardsHijackTarget(const TObjectPtr<AEnemy>& InHijackTarget);
	void ProcessFlyingTowardsHijackTarget();
	void ReachedHijackTarget();
	void ChangeActorRenderState(const bool bState);
	void SetupFlyingTowardsColumnLocation();
	void StandUp() const;
};
