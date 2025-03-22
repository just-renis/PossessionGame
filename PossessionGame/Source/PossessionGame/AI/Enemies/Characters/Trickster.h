#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Categories/SpecialEnemy.h"
#include "Trickster.generated.h"

DECLARE_DELEGATE_TwoParams(FOnWeaponShootStateChange, bool, bool);

UCLASS()
class POSSESSIONGAME_API ATrickster : public ASpecialEnemy
{
	GENERATED_BODY()
public:
	FOnWeaponShootStateChange OnWeaponShootStateChange;

	void ThrowPoisonGrenade();
	void ThrowEmpGrenade();
private:
	ATrickster();
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_TriggerDeathVoiceLine() override;

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