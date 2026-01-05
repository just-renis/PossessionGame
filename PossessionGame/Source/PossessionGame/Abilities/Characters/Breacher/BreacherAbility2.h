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
	
	virtual float AI_SpecialCooldownMultiplier() override;

	virtual bool IsProprietorPossessed() override;

	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
	virtual void CooldownLogic(const float InDeltaTime) override;
	virtual void Setup(TObjectPtr<AEnemy> InAbilityOwner, TObjectPtr<UAbilityData> InAbilityData, TObjectPtr<UDelegateManager> InDelegateManager) override;
	void RechargeAbility();
	virtual void Destroy() override;
};
