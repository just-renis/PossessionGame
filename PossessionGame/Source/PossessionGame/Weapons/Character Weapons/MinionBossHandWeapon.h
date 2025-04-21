#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "MinionBossHandWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AMinionBossHandWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	AMinionBossHandWeapon();

	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_MinionBossWeaponMaterial;

	virtual void AI_Shoot() override;

	virtual void BeginPlay() override;
	virtual void SetMeshes(const EWeapon InPrimaryDataAssetName) override;
};
