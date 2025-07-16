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
	virtual void AI_Reload() override;
	virtual void AI_Shoot() override;
	
	virtual void P_Shoot(bool bIsFirstWeapon) override;
	virtual void P_Reload(int32 OtherAmmoSource) override;

	virtual void BeginPlay() override;
};
