#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "RangerWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ARangerWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
public:
	ARangerWeapon();
private:
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> GrenadeLaunchSpecialComponent;
	
	virtual void AI_Reload() override;
	virtual void AI_Shoot() override;
	
	virtual float P_UpgradeDamageAppliance() override;
	
	virtual void P_Shoot() override;
	virtual void P_Reload(int32 OtherAmmoSource) override;
	virtual void P_Setup() override;

	virtual void BeginPlay() override;
	void AddSpecialUpgradeLogic(const EUpgrade InUpgradeName, const EUpgradeAttribute InSpecialAttributeName);
	void AttachGrenadeLaunchSpecialVfx();
	void ChangeGrenadeLaunchSpecialComponentState(const bool bState) const;
};
