#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Trickster.generated.h"

DECLARE_DELEGATE_TwoParams(FOnWeaponShootStateChange, bool, bool);

UCLASS()
class POSSESSIONGAME_API ATrickster : public AEnemy
{
	GENERATED_BODY()
public:
	FOnWeaponShootStateChange& GetOnWeaponShootStateChangeDelegate() { return OnWeaponShootStateChange; }
	
	void P_ThrowPoisonGrenade() const;
	void P_ThrowEmpGrenade() const;
private:
	ATrickster();
	
	FOnWeaponShootStateChange OnWeaponShootStateChange;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;

	virtual void P_Setup() override;
	virtual void P_SetupRightClickInputStartedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) override;
	virtual void P_SetupRightClickInputTriggeredState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) override;
	virtual void P_SetupRightClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) override;
	virtual void P_SetupLeftClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	virtual void P_SetupLeftClickInputStartedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	virtual void P_SetupColorsForUI() override;

	virtual void BeginPlay() override;
	void StartedShootingWeapon(bool bFirstWeapon);
	void StoppedShootingWeapon(bool bFirstWeapon);
};