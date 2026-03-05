#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "TankAbility3.generated.h"

class ATank;

UCLASS()
class POSSESSIONGAME_API UTankAbility3 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void P_Execute() override;
private:
	float ForwardImpulseStrength = 2500.f;
	float UpImpulseStrength = 1400.f;
	UPROPERTY()
	TObjectPtr<ATank> Proprietor;
	
	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) override;
};
