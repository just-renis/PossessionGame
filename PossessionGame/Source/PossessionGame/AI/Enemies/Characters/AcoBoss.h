#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "AcoBoss.generated.h"

UCLASS()
class POSSESSIONGAME_API AAcoBoss : public AEnemy
{
	GENERATED_BODY()

	AAcoBoss();

	FTimerHandle FlyingTowardsHijackEnemyHandle;
	TArray<ECharacterType> CharactersToHijack = { ECharacterType::Guardian, ECharacterType::Trickster, ECharacterType::Ranger, ECharacterType::Wraith,
		ECharacterType::Breacher, ECharacterType::Spy, ECharacterType::Tank, ECharacterType::Samurai };
	UPROPERTY(EditDefaultsOnly, Category = "First Phase", meta = (AllowPrivateAccess = true))
	float MovementSpeedTowardsHijackTarget = 5.f;
	UPROPERTY()
	TObjectPtr<AActor> HijackTargetEnemy;
	
	virtual void AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection) override;

	virtual void BeginPlay() override;
	virtual void ApplyEmpEffect(const float InEffectDuration) override;
	virtual void ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent) override;
	virtual void InitiateDeath() override;
	void SpawnFirstPhaseEnemy();
	void SetupFlyingTowardsHijackTarget(const TObjectPtr<AActor>& InHijackTarget);
	void ProcessFlyingTowardsHijackTarget();
};
