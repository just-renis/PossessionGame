#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "BreacherAbility3.generated.h"

class ABreacher;

UCLASS()
class POSSESSIONGAME_API UBreacherAbility3 : public UAbility
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
