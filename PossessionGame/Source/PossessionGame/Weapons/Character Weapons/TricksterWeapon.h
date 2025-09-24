#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/DualWieldingWeapon.h"
#include "TricksterWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ATricksterWeapon : public ADualWieldingWeapon
{
	GENERATED_BODY()
public:
	void P_StopOverChargeVfx() const;
private:
	ATricksterWeapon();

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> FirstWeaponOverChargeComponent;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SecondWeaponOverChargeComponent;

	virtual void AI_Reload() override;

	virtual void P_Reload(int32 OtherAmmoSource) override;
	void P_PlayOverChargeVfx();
	
	virtual void BeginPlay() override;
	virtual void SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh) override;
	virtual void SetMeshes(const EWeapon InPrimaryDataAssetName) override;
	virtual void ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, const bool bFromFirstWeapon) override;
};