#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Guardian.generated.h"

UCLASS()
class POSSESSIONGAME_API AGuardian : public AEnemy
{
	GENERATED_BODY()
public:
	virtual FLinearColor GetDamageNumberColor() const override;
	
	void AI_SetFirstAbilityUsageState(const bool bState) { bIsFirstAbilityInUse = bState; }
	void AI_ToggleShieldVfx(const bool bState);
	void AI_ToggleMeshShieldAbilityState(const bool bState);
	
	void SetCustomTimeDilation(const float InDilation);
	void ToggleShield(const bool bState);
	void SetReflectDamageState(const bool bState) { bReflectDamageEnabled = bState; }
protected:
	AGuardian();
private:
	bool bReflectDamageEnabled = false;
	bool bIsShieldDisappearing = false;
	bool bImmuneToFirstHit = false;
	bool bIsFirstAbilityInUse = false;
	float ShieldColorDensity = 5.f;
	float ShieldRadialRadius = 1.f;
	float Radius = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Guardian;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Shield;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Shield;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_EnemyAbilityShield;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> AI_GuardianShieldComponent;
	UPROPERTY()
	FTimerHandle ShieldUpdateHandle;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;
	virtual void AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage) override;

	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputTriggeredState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override {}
	virtual void P_SetupLeftClickInputStartedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	virtual void P_SetupLeftClickInputCompletedState(const TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent) override;
	void P_OnShootStarted();
	void P_ResetShootHoldDuration() const;
	void P_CancelShootingHold();
	virtual void P_InitiateJump() override;
	virtual void P_InitiateStomp() override;
	virtual void P_Move(const FInputActionValue& Value) override;
	void P_UpdateShieldMaterialAppear();
	void P_UpdateShieldMaterialDisappear();
	void P_UpdateShieldMaterial(const bool bState);
	void P_ResetShieldValues();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void BeginPlay() override;
	virtual void SetMesh(const ECharacterType PrimaryDataAssetName) override;
};
