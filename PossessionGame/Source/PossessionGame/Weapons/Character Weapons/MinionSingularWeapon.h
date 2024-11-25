#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "MinionSingularWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AMinionSingularWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
	
	virtual void AI_Shoot() override { ShootProjectile(EPoolObject::RangerProjectile); }

	virtual void BeginPlay() override;
	virtual void SetMeshes(EWeapon PrimaryDataAssetName) override;
};
