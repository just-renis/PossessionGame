#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "TankAbility1.generated.h"

class ATank;

UCLASS()
class POSSESSIONGAME_API UTankAbility1 : public UAbility
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<ATank> Proprietor;
	
	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) override;
};
