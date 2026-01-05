#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "BreacherAbility1.generated.h"

class ABreacher;

UCLASS()
class POSSESSIONGAME_API UBreacherAbility1 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void P_Execute() override;
private:
	UPROPERTY()
	TObjectPtr<ABreacher> Proprietor;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
};
