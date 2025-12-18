#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Interfaces/DamageReasonProvider.h"
#include "Guardian.generated.h"

enum class EPoolObject : uint8;

UCLASS()
class POSSESSIONGAME_API AGuardian : public AEnemy, public IDamageReasonProvider
{
	GENERATED_BODY()
public:
	virtual EDamageReason GetDamageReason() const override;
	virtual FLinearColor GetDamageNumberColor() const override;
	TObjectPtr<UMaterialInterface> GetTimeStopMaterial() const { return MI_TimeStop; }
	
	void AI_ToggleShieldVfx(const bool bState);
	void AI_ToggleMeshShieldAbilityState(const bool bState);

	void P_LaunchVectorBlast();

	void SetCustomTimeDilation(const float InDilation);
	void ToggleShield(const bool bState);
	void SetReflectDamageState(const bool bState) { bReflectDamageEnabled = bState; }
	void SetFirstAbilityUsageState(const bool bState) { bIsFirstAbilityInUse = bState; }
	void SetSecondAbilityUsageState(const bool bState) { bIsSecondAbilityInUse = bState; }
protected:
	AGuardian();
private:
	bool bReflectDamageEnabled = false;
	bool bIsShieldDisappearing = false;
	bool bImmuneToFirstHit = false;
	bool bIsFirstAbilityInUse = false;
	bool bIsSecondAbilityInUse = false;
	float ShieldColorDensity = 5.f;
	float ShieldRadialRadius = 1.f;
	float Radius = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Guardian;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Shield;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_TimeStop;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Shield;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_EnemyAbilityShield;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> GuardianShieldComponent;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VectorBlastMuzzleComponent;
	UPROPERTY()
	FTimerHandle ShieldUpdateHandle;
	UPROPERTY()
	FTimerHandle ShootSecondWeaponProjectileHandle;

	bool IsSecondAbilityInUse() const { return bIsSecondAbilityInUse; }
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathElectrification() override;
	virtual void AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage) override;

	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputTriggeredState() override {}
	virtual void P_SetupLeftClickInputStartedState() override;
	virtual void P_SetupLeftClickInputCompletedState() override;
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
	void P_ToggleShieldVfx(const bool bState);
	virtual void P_ShowReceivedDamageNumber(const float InDamage) override;
	virtual void P_WeaponShoot() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void BeginPlay() override;
	virtual void SetMesh(const ECharacterType PrimaryDataAssetName) override;
	virtual void DisableAllVfxAfterPossession() override;
	void ToggleShieldVfx(const bool bState, const EVfx InVfxName);
	virtual void InitiateDeath() override;
	UFUNCTION()
	void ShootSecondWeaponProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, bool bFromFirstWeapon);
};
