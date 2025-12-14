#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/DualWieldingWeapon.h"
#include "GuardianWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AGuardianWeapon : public ADualWieldingWeapon
{
	GENERATED_BODY()
public:
	float P_GetShootingHoldDuration() const { return ShootingHoldDuration; }
	
	void P_ResetShootingHoldDuration() { ShootingHoldDuration = 0.f; }
	void P_ShootingHoldStarted() { bIsShootingHolding = true; }
	void P_CancelShootingHold();
	
	void ToggleHandsMeshAbilityState(const bool bState) const;
private:
	bool bIsShootingHolding = false;
	float ShootingHoldDuration = 0.f;
	FTimerHandle SecondWeaponShotHandle;
	FTimerHandle ShootingHoldDurationHandle;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_Hands;
	
	virtual bool P_CanShoot() override;
	
	virtual void P_UpdateAmmoTextHUD() override;
	virtual void P_Shoot(const EPoolObject InProjectileName) override;
	void P_UpdateShootingHoldDuration();

	virtual void BeginPlay() override;
	virtual void SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh) override;
	virtual void SetMeshes(const EWeapon InPrimaryDataAssetName) override;
	virtual void SetCustomDilation(const float InDilation) override;
	void ShootSecondWeaponShot() const;
	virtual void DestroyThis() override;
	virtual void PostProprietorSetup() override;
};
