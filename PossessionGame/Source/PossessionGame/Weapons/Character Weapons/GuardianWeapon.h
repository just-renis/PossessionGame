#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/DualWieldingWeapon.h"
#include "GuardianWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AGuardianWeapon : public ADualWieldingWeapon
{
	GENERATED_BODY()
public:
	void PlayOverChargeVfx();
	void StopOverChargeVfx() const;
private:
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> FirstWeaponOverChargeComponent;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SecondWeaponOverChargeComponent;
	
	virtual bool P_CanShoot(const bool InbIsFirstWeapon) override;
	virtual void P_UpdateAmmoTextHUD() override;
	virtual void P_Shoot(const bool bIsFirstWeapon) override;

	virtual void BeginPlay() override;
	virtual void SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh) override;
	virtual void SetMeshes(const EWeapon InPrimaryDataAssetName) override;
};
