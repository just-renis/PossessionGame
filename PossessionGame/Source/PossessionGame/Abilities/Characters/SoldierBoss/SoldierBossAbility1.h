#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "SoldierBossAbility1.generated.h"

class ASoldierBoss;

UCLASS()
class POSSESSIONGAME_API USoldierBossAbility1 : public UAbility
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ASoldierBoss> Proprietor;

	virtual void AI_Execute() override;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
	virtual void CooldownLogic(const float InDeltaTime) override;
};
