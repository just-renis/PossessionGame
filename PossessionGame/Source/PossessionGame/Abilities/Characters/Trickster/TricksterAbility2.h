#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "TricksterAbility2.generated.h"

class ATrickster;

UCLASS()
class POSSESSIONGAME_API UTricksterAbility2 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void P_Execute() override;
private:
	UPROPERTY()
	TObjectPtr<ATrickster> Proprietor;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) override;
	UFUNCTION()
	void ThrowEMPGrenade() const;
};
