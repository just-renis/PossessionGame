#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "RangerAbility3.generated.h"

class ARanger;

UCLASS()
class POSSESSIONGAME_API URangerAbility3 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void P_Execute() override;
	virtual void P_StopAbility() override;
private:
	UPROPERTY()
	TObjectPtr<ARanger> Proprietor;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) override;
};
