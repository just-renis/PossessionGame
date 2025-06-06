#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "GuardianSingularWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AGuardianSingularWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
public:
	void CancelShootingHold();
	void ResetShootingHoldDuration() { ShootingHoldDuration = 0.f; }
	void ShootingHoldStarted() { bIsShootingHolding = true; }
	void ToggleMeshAbilityState(const bool bState) const;
private:
	AGuardianSingularWeapon();
	
	bool bIsShootingHolding = false;
	float ShootingHoldDuration = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_GuardianWeaponMaterial;
	//UPROPERTY()
	//TObjectPtr<UMaterialInstanceDynamic> MID_GuardianWeaponMaterial;
	
	virtual void AI_Shoot() override;

	virtual float P_ReceiveHitScanDamage() override;

	virtual void P_Shoot(const bool bIsFirstWeapon) override;
	virtual void P_ApplyCurrentVariation() override;

	virtual void BeginPlay() override;
	virtual void SetMeshes(EWeapon PrimaryDataAssetName) override;
	virtual void Tick(const float InDeltaSeconds) override;
};
