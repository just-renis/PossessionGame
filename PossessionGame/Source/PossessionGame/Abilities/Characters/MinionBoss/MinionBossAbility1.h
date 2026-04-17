#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "MinionBossAbility1.generated.h"

class UNiagaraComponent;
class AMinionBoss;

UCLASS()
class POSSESSIONGAME_API UMinionBossAbility1 : public UAbility
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AMinionBoss> Proprietor;
	
	virtual void AI_Execute() override;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
	virtual void CooldownLogic(const float InDeltaTime) override;
};
