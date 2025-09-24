#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "WardenAbility1.generated.h"

class AWarden;

UCLASS()
class POSSESSIONGAME_API UWardenAbility1 : public UAbility
{
	GENERATED_BODY()

	float TriggerBeamEndTime = 1.5f;
	UPROPERTY()
	TObjectPtr<AWarden> Proprietor;
	UPROPERTY()
	FTimerHandle BeamHandle;

	virtual void AI_Execute() override;
	virtual void AI_StopAbility() override;
	
	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) override;
	void TriggerBeamEnd() const;
};
