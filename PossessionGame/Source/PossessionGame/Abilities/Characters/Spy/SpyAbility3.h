#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "SpyAbility3.generated.h"

class ASpy;

UCLASS()
class POSSESSIONGAME_API USpyAbility3 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void P_Execute() override;
	virtual void P_StopAbility() override;
private:
	UPROPERTY()
	TObjectPtr<ASpy> Proprietor;
	
	bool IsProprietorPossessed();
	
	void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor);
};