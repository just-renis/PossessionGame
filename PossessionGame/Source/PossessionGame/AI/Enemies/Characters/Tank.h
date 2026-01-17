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
	ATank();

	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	
	virtual void AI_ReloadLogic() override;
	virtual void AI_SetCanShoot(const bool bState) override;
	virtual void AI_PlayShootMontage() override;
	virtual void AI_SetupWeapon() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathSound() override;

	bool P_GetIsLeftOrRightInputNotPressed();
	
	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputStartedState() override;
	virtual void P_SetupLeftClickInputCompletedState() override;
	virtual void P_SetupShiftInput() override {}
	virtual void P_Destroy() override;
	void P_ToggleChargeMinigun(const bool bState);
	void P_StopChargingMinigun();
	virtual void P_WeaponShoot() override;
	UFUNCTION()
	void P_StopMinigun();
	
	virtual void BeginPlay() override;
};