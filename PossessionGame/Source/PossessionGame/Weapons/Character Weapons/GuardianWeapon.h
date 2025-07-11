#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/DualWieldingWeapon.h"
#include "GuardianWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AGuardianWeapon : public ADualWieldingWeapon
{
	GENERATED_BODY()
public:
	void CancelShootingHold() const;
	void ShootingHoldStarted() const;
	void ResetShootingHoldDuration() const;
	void ToggleHandsMeshAbilityState(const bool bState) const;
private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_Hands;
	UPROPERTY()
	FTimerHandle SecondWeaponShotHandle;
	
	virtual bool P_CanShoot(const bool InbIsFirstWeapon) override;
	
	virtual void P_UpdateAmmoTextHUD() override;
	virtual void P_Shoot(const bool bIsFirstWeapon) override;

	virtual void BeginPlay() override;
	virtual void SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh) override;
	virtual void SetMeshes(const EWeapon InPrimaryDataAssetName) override;
	virtual void SetCustomDilation(const float InDilation) override;
	void ShootSecondWeaponShot() const;
	virtual void DestroyThis() override;
};
