#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Categories/AssaultEnemy.h"
#include "Ranger.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ARanger : public AAssaultEnemy
{
	GENERATED_BODY()
public:
	void LaunchGrenade();
	void SpawnShield();
	void SetCustomTimeDilation(float Dilation);
private:
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> GrenadeLauncherMuzzleFlashComponent;
	
	virtual void AI_SetupWeapon() override;

	virtual void P_Setup() override;
	virtual void P_SetupRightClickInputStartedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) override;
	virtual void P_SetupRightClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent) override;
	void P_ChangeAiming(bool bIsZooming);
	virtual void P_SetupColorsForUI() override;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SpawnStopTimeAbilityVfx(EVfx VfxName);
	void AttachOrLoadGrenadeLauncherMuzzle();
	void AttachGrenadeLauncherMuzzle();
	void GrenadeLauncherMuzzleFlashLogic();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
