#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "TricksterAbility3.generated.h"

class ATrickster;

UCLASS()
class POSSESSIONGAME_API UTricksterAbility3 : public UAbility
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ATrickster> Proprietor;

	virtual void P_Execute() override;
	virtual void P_StopAbility() override;

	virtual bool IsProprietorPossessed() override;

	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
};
