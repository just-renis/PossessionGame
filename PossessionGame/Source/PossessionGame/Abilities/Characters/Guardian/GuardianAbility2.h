#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "GuardianAbility2.generated.h"

class AGuardian;

UCLASS()
class POSSESSIONGAME_API UGuardianAbility2 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void P_Execute() override;
	virtual void P_StopAbility() override;
private:
	float Dilation = 3.f;
	UPROPERTY()
	TObjectPtr<AGuardian> Proprietor;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) override;
};
