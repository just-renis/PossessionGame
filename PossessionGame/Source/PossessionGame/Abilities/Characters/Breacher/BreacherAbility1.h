#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "BreacherAbility1.generated.h"

class ABreacher;

UCLASS()
class POSSESSIONGAME_API UBreacherAbility1 : public UAbility
{
	GENERATED_BODY()

	
	UPROPERTY()
	TObjectPtr<ABreacher> Proprietor;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) override;
};
