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
	bool bRangerBlueVariationEnabled = false;
	UPROPERTY()
	TObjectPtr<ARanger> Proprietor;

	virtual float AI_GetCooldownReduction() override;

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
	virtual void CooldownLogic(const float InDeltaTime) override;
	virtual void SetupUpgradeAppliance(const bool bIsPlayer) override;
	virtual void Setup(TObjectPtr<AEnemy> InAbilityOwner, TObjectPtr<UAbilityData> InAbilityData, TObjectPtr<UDelegateManager> InDelegateManager) override;
};
