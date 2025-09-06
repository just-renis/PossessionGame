#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "GuardianAbility2.generated.h"

class AGuardian;

UCLASS()
class POSSESSIONGAME_API UGuardianAbility2 : public UAbility
{
	GENERATED_BODY()
public:
	virtual void P_Execute() override;
	virtual void P_StopAbility() override;
private:
	float Dilation = 2.5f;
	float TimeStopPulseDistance = 0.f;
	UPROPERTY()
	TObjectPtr<AGuardian> Proprietor;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_TimeStop;
	UPROPERTY()
	FTimerHandle ProcessTimeStopHandle;

	void P_TimeStopProcess();
	void P_TimeStopRegressProcess();
	void P_StopTimeStopProcess();

	virtual bool IsProprietorPossessed() override;
	
	virtual void SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) override;
	void SetTimeStopProcess();
};
