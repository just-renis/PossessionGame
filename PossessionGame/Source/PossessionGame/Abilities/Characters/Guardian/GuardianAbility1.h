#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "GuardianAbility1.generated.h"

class AGuardian;

UCLASS()
class POSSESSIONGAME_API UGuardianAbility1 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void AI_Execute() override;
	virtual void AI_StopAbility() override;
	
	virtual void P_Execute() override;
	virtual void P_StopAbility() override;
private:
	bool bReflectDamageEnabled = false;
	float TotalDamageReduction;
	UPROPERTY()
	TObjectPtr<AGuardian> Proprietor;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
	virtual void SetupUpgradeAppliance(const bool bIsPlayer) override;
};
