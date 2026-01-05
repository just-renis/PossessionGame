#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "GuardianAbility3.generated.h"

class AGuardian;

UCLASS()
class POSSESSIONGAME_API UGuardianAbility3 : public UAbility
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AGuardian> Proprietor;

	virtual void P_Execute() override;
	
	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
};
