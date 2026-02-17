#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Spy.generated.h"

UCLASS()
class POSSESSIONGAME_API ASpy : public AEnemy
{
	GENERATED_BODY()
public:
	bool GetXrayState() const { return bXrayEnabled; }
	
	void SetXrayState(const bool bState) { bXrayEnabled = bState; }
private:
	bool bXrayEnabled = false;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_SetController() override;

	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputStartedState() override;
	virtual void P_SetupLeftClickInputTriggeredState() override {}
	virtual void P_WeaponShoot() override;
	
	virtual void BeginPlay() override;
};