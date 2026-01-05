#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "MinionBossAbility2.generated.h"

class AMinionBoss;

UCLASS()
class POSSESSIONGAME_API UMinionBossAbility2 : public UAbility
{
	GENERATED_BODY()

	bool bIsLaserDisabled = false;
	bool bLaserUpdateFirstFrame = true;
	bool bIsLaserActive = false;
	float Acceleration = 0.05f;
	float MaxSpeed = 15.f;
	float MinSpeed = 0.5f;
	float CurrentRotationSpeed = 0.5f;
	float LaserActivationFromExecution = 1.65f;
	float PlayerPositionReceiveBeforeLaserActivation = 0.3f;
	float LaserBallDuration = 7.f;
	float LaserDuration = 6.f;
	FVector PlayerLocationAtInitiation;
	FRotator CurrentAngularVelocity = FRotator::ZeroRotator;
	FTimerHandle ShootLaserHandle;
	FTimerHandle UpdateLaserLocationHandle;
	FTimerHandle ReceivePlayerPositionHandle;
	FTimerHandle TurnOffLaserHandle;
	UPROPERTY()
	TObjectPtr<AMinionBoss> Proprietor;

	virtual void AI_Execute() override;
	virtual void AI_StopAbility() override;

	bool GetLaserActivityState() const { return bIsLaserActive; }
	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
	virtual void CooldownLogic(const float InDeltaTime) override;
	void StopLaser();
	void TurnOffLaserEye() const;
	void DisableLaser();
	void ShootLaser();
	void UpdateLaserEndLocation();
	void ReceivePlayerPosition();
	void TurnOffLaser();
};
