#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "WraithAbility2.generated.h"

class AWraith;

UCLASS()
class POSSESSIONGAME_API UWraithAbility2 : public UAbility
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<AWraith> Proprietor;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
};
