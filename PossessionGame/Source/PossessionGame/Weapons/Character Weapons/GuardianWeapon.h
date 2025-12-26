#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/DualWieldingWeapon.h"
#include "GuardianWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AGuardianWeapon : public ADualWieldingWeapon
{
	GENERATED_BODY()
public:
	void ToggleHandsMeshAbilityState(const bool bState) const;
private:
	FTimerHandle SecondWeaponShotHandle;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_Hands;
	
	virtual bool P_CanShoot() override;
	
	virtual void P_UpdateAmmoTextHUD() override;
	virtual void P_Shoot(const EPoolObject InProjectileName) override;

	virtual void BeginPlay() override;
	virtual void SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh) override;
	virtual void SetMeshes(const EWeapon InPrimaryDataAssetName) override;
	virtual void SetCustomDilation(const float InDilation) override;
	void ShootSecondWeaponShot();
	virtual void DestroyThis() override;
};
