#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "GuardianSingularWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AGuardianSingularWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	AGuardianSingularWeapon();
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_GuardianWeaponMaterial;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> VectorBlastMuzzleComponent;
	
	virtual void AI_Shoot() override;

	virtual void P_Shoot() override;

	virtual void BeginPlay() override;
	virtual void SetMeshes(EWeapon PrimaryDataAssetName) override;
	virtual void MuzzleLogicFromAbility() override;
};
