#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "BreacherAbility2.generated.h"

class ABreacher;

UCLASS()
class POSSESSIONGAME_API UBreacherAbility2 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void AI_Execute() override;
	
	virtual void P_Execute() override;
private:
	UPROPERTY()
	TObjectPtr<ABreacher> Proprietor;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) override;
	virtual void CooldownLogic(const float InDeltaTime) override;
};
