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
	virtual void P_Execute() override;
	virtual void P_StopAbility() override;
private:
	float BaseDamageReduction = 0.8f;
	float TotalDamageReduction;
	UPROPERTY()
	TObjectPtr<AGuardian> Proprietor;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) override;
};
