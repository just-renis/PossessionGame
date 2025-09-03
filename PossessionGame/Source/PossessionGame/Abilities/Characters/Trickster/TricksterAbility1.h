#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "TricksterAbility1.generated.h"

class ATrickster;

UCLASS()
class POSSESSIONGAME_API UTricksterAbility1 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void AI_Execute() override;
	
	virtual void P_Execute() override;
private:
	UPROPERTY()
	TObjectPtr<ATrickster> Proprietor;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) override;
	virtual void CooldownLogic(const float InDeltaTime) override;

};
