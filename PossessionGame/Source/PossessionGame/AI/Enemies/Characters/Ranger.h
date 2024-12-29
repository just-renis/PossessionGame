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
	
	void AI_ToggleGrenadeLaunchTrigger(const bool bState) const;
	
	void LaunchGrenade();
	void SpawnShield();
	void SetCustomTimeDilation(float Dilation);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Grenade Launcher", meta = (AllowPrivateAccess = true))
	float GrenadeLaunchTriggerRadiusInMetres = 10.f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> GrenadeLauncherMuzzleFlashComponent;
	UPROPERTY()
	TObjectPtr<ATriggerSphere> AIGrenadeLaunchTriggerSphere;

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
	UFUNCTION()
	void TryTriggeringGrenadeLaunch();
};
