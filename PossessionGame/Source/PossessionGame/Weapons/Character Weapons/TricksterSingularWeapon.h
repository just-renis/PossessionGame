#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "TricksterSingularWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ATricksterSingularWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
	
	virtual void AI_Shoot() override { ShootProjectile(EPoolObject::RangerProjectile); }

	virtual bool P_CanReload(int32 OtherAmmoSource) override;
	virtual void P_Shoot(bool bIsFirstWeapon) override;

	virtual void BeginPlay() override;
	virtual void SetMeshes(EWeapon PrimaryDataAssetName) override;
};