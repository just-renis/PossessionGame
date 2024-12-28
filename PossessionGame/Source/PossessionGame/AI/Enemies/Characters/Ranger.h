#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Categories/AssaultEnemy.h"
#include "Ranger.generated.h"

class ATriggerSphere;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ARanger : public AAssaultEnemy
{
	GENERATED_BODY()
public:
	ARanger();
	
	void LaunchGrenade();
	void SpawnShield();
	void SetCustomTimeDilation(float Dilation);
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<ATriggerSphere> AIGrenadeLaunchTriggerSphere;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> GrenadeLauncherMuzzleFlashComponent;

	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	UFUNCTION()
	void AI_OnPlayerEnterGrenadeLaunchTrigger(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void AI_PlaySpawnSound();

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
	virtual void LoadSounds() override;
	virtual void PossessedBy(AController* NewController) override;
	void SetupGrenadeLaunchTriggerSphere();
	void DestroyGrenadeLaunchTriggerSphere() const;
};
