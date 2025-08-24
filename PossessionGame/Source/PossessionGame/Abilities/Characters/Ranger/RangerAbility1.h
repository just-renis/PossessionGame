#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "RangerAbility1.generated.h"

class ARanger;

UCLASS()
class POSSESSIONGAME_API URangerAbility1 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void AI_Execute() override;
	
	virtual void P_Execute() override;
private:
	bool bFirstSpecialUpgradeEnabled = false;
	UPROPERTY()
	TObjectPtr<ARanger> Proprietor;

	virtual float AI_GetCooldownReduction() override;

	virtual void P_ApplyVariation() override;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) override;
	virtual void CooldownLogic(const float DeltaTime) override;
	virtual void SetupUpgradeAppliance(const bool bIsPlayer) override;
};
