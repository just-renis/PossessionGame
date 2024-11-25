#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Categories/HeavyEnemy.h"
#include "Tank.generated.h"

UCLASS()
class POSSESSIONGAME_API ATank : public AHeavyEnemy
{
	GENERATED_BODY()
public:
	virtual void P_Shoot(bool bFirstWeapon) override;
private:
	virtual void AI_SetupWeapon() override;

	bool P_GetIsLeftOrRightInputNotPressed();
	
	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputStartedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	virtual void P_SetupLeftClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	virtual void P_SetupRightClickInputStartedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) override;
	virtual void P_SetupRightClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) override;
	virtual void P_DisableEnemy() override;
	void P_ToggleChargeMinigun(bool bState);
	void P_StopChargingMinigun();
	virtual void P_SetupColorsForUI() override;
	UFUNCTION()
	void P_StopMinigun();
	
	virtual void BeginPlay() override;
};