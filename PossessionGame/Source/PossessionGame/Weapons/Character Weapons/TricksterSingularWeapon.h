#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "TricksterSingularWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ATricksterSingularWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	ATricksterSingularWeapon();

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> FpsTrailVfx;
	
	virtual void AI_Shoot() override;

	virtual bool P_CanReload(int32 OtherAmmoSource) override;
	virtual float P_ReceiveHitScanDamage() override;
	
	virtual void P_Shoot() override;

	virtual void BeginPlay() override;
	virtual void SetMeshes(EWeapon PrimaryDataAssetName) override;
	virtual void FpsTrailVfxLogic() override;
};