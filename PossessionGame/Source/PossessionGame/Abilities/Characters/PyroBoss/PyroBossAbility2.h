#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PyroBossAbility2.generated.h"

class APyroBoss;

UCLASS()
class POSSESSIONGAME_API UPyroBossAbility2 : public UAbility
{
	GENERATED_BODY()

	bool bFirstStompSucceed = false;
	FTimerHandle FirstStompHandle;
	FTimerHandle SecondStompHandle;
	UPROPERTY()
	TObjectPtr<APyroBoss> Proprietor;

	virtual void AI_Execute() override;
	virtual void AI_StopAbility() override;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
	void StompHitTheGround();
	virtual void CooldownLogic(const float InDeltaTime) override;
};
