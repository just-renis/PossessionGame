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
	FVector PlayerLocationAtInitiation;
	UPROPERTY()
	TObjectPtr<AMinionBoss> Proprietor;

	virtual void AI_Execute() override;
	virtual void AI_StopAbility() override;
	
	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) override;
	virtual void CooldownLogic(const float InDeltaTime) override;
	void StopLaser();
	void TurnOffLaser();
	void DisableLaser();
	UFUNCTION()
	void ShootLaser();
	UFUNCTION()
	void UpdateLaserEndLocation();
};
