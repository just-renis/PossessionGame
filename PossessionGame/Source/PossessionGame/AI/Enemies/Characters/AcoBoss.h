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
	void InitiateFight();
private:
	AAcoBoss();

	bool bIsFlyingTowardsHijackTarget = false;
	bool bIsUltraInstinctActive = false;
	float UltraInstinctCooldown = 5.f;
	float UltraInstinctTimer = 0.f;
	float UltraInstinctHoldPose = 2.f;
	float UltraInstinctHoldTimer = 0.f;
	const float FrameRate = 1.f / 60.f;
	int32 CountdownSecs = 6.f;
	int32 CurrentUltraInstinctPose = 0;
	FVector LookAtLocation;
	FTimerHandle FlyingTowardsTargetHandle;
	FTimerHandle CountdownTimerHandle;
	FTimerHandle AbilityCycleHandle;
	TArray<ECharacterType> CharactersToHijack = { /*ECharacterType::Guardian, ECharacterType::Trickster, ECharacterType::Ranger, ECharacterType::Wraith,
		ECharacterType::Breacher, ECharacterType::Spy, ECharacterType::Tank, ECharacterType::Samurai*/ ECharacterType::Ranger};
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
	void ProcessFlyingTowardsTarget();
	void ReachedHijackTarget();
	void ChangeActorRenderState(const bool bState);
	void SetupFlyingTowardsColumnLocation();
	void StandUp() const;
	void FirstPhaseFinished();
	void InitiateSecondPhase();
	void TriggerSlamImpact();
	void SetupSecondPhaseStartCountdown();
	void UpdateSecondPhaseCountdown();
	void ProcessAbilityCycle();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
