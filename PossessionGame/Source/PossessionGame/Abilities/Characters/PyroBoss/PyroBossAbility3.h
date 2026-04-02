#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PyroBossAbility3.generated.h"

class APyroBoss;

UCLASS()
class POSSESSIONGAME_API UPyroBossAbility3 : public UAbility
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<APyroBoss> Proprietor;

	virtual void AI_Execute() override;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
	virtual void CooldownLogic(const float InDeltaTime) override;
};
