#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Tank.generated.h"

UCLASS()
class POSSESSIONGAME_API ATank : public AEnemy
{
	GENERATED_BODY()
public:
	virtual void P_Shoot() override;
private:
	virtual void AI_ReloadLogic() override;
	virtual void AI_SetCanShoot(const bool bState) override;
	virtual void AI_PlayShootMontage() override;
	virtual void AI_SetupWeapon() override;

	bool P_GetIsLeftOrRightInputNotPressed();
	
	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputStartedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	virtual void P_SetupLeftClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	virtual void P_Destroy() override;
	void P_ToggleChargeMinigun(const bool bState);
	void P_StopChargingMinigun();
	virtual void P_SetupColorsForUI() override;
	UFUNCTION()
	void P_StopMinigun();
	
	virtual void BeginPlay() override;
};