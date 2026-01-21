#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "WraithAbility3.generated.h"

class AWraith;

UCLASS()
class POSSESSIONGAME_API UWraithAbility3 : public UAbility
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AWraith> Proprietor;

	virtual void AI_Execute() override;

	virtual void P_Execute() override;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
	virtual void CooldownLogic(const float InDeltaTime) override;
};
