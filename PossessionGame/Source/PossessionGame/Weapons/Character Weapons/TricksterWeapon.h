#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/DualWieldingWeapon.h"
#include "TricksterWeapon.generated.h"

USTRUCT()
struct FShootMontageHandler
{
	GENERATED_BODY()

	bool bIsFirstWeaponShooting = false;
	bool bIsSecondWeaponShooting = false;
};

UCLASS()
class POSSESSIONGAME_API ATricksterWeapon : public ADualWieldingWeapon
{
	GENERATED_BODY()
public:
	void P_SetupOnShootState();
private:
	ATricksterWeapon();
	
	UPROPERTY()
	FShootMontageHandler ShootMontageHandler;

	virtual void AI_Reload() override;
	
	virtual void P_Reload(int32 OtherAmmoSource) override;
	void P_SetShootMontage(bool bFirstWeapon, bool bState);
	
	virtual void BeginPlay() override;
	virtual void SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh) override;
	virtual void SetMeshes(EWeapon PrimaryDataAssetName) override;
	virtual void PlayShootMontage() override;
	virtual void AttachMuzzleFlash(const EVfx InVfxName) override;
	virtual void ShootProjectile(const EPoolObject InProjectileName, const bool bOverrideRotation, const bool bFromFirstWeapon) override;
};