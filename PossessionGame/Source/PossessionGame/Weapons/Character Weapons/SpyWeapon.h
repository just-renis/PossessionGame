#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "SpyWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ASpyWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
public:
	void SetInvincibilityState(const bool bState);
private:
	ASpyWeapon();
	
	float MaterialEmissiveEffect = 0.f;
	float MaterialWidth = 0.f;
	float MaterialHue = 0.f;
	float MaterialColorStrength = 1.f;
	int32 MaterialChangeCountLoops = 0;
	FTimerHandle ProcessInvincibilityMaterialHandle;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_DefaultMaterial;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ShootSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ReloadSfx;

	virtual void AI_Reload() override;
	virtual void AI_Shoot() override;
	
	virtual void P_Shoot(const EPoolObject InProjectileName) override;
	virtual void P_Reload() override;
	virtual void P_Setup() override;
	
	virtual void BeginPlay() override;
	virtual void HitScanLogic() override;
	void ProcessInvincibilityMaterialActivation();
	void ProcessInvincibilityMaterialDeactivation();
};