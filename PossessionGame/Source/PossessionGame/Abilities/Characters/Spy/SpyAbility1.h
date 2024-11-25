#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "SpyAbility1.generated.h"

class ASpy;

UCLASS()
class POSSESSIONGAME_API USpyAbility1 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void AI_Execute() override;
	virtual void AI_StopAbility() override;

	virtual void P_Execute() override;
	virtual void P_StopAbility() override;
private:
	UPROPERTY()
	TObjectPtr<ASpy> Proprietor;
	
	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) override;
};