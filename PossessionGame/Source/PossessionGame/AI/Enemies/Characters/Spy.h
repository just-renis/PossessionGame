#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Interfaces/Concealable.h"
#include "Spy.generated.h"

UCLASS()
class POSSESSIONGAME_API ASpy : public AEnemy, public IConcealable
{
	GENERATED_BODY()
public:
	bool GetXRayState() { return bXRayEnabled; }
	
	virtual void SetConceal(bool bState) override;
	void SetXRayState(bool bState) { bXRayEnabled = bState; }
	void StopConcealingAbility(bool bIsPlayer = true);
private:
	bool bXRayEnabled = false;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_ShootLogic() override;
	virtual void AI_SetController() override;

	virtual void P_Setup() override;
	virtual void P_Shoot() override;
	virtual void P_SetupLeftClickInputStartedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	virtual void P_SetupLeftClickInputTriggeredState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override {}
	virtual void P_SetupColorsForUI() override;
	
	virtual void BeginPlay() override;
};