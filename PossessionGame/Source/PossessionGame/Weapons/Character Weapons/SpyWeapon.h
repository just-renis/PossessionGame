#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Interfaces/Concealable.h"
#include "PossessionGame/Interfaces/Concealable.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "SpyWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoPercentageChanged, int32);

UCLASS()
class POSSESSIONGAME_API ASpyWeapon : public AOneHandedWeapon, public IConcealable
{
	GENERATED_BODY()
public:
	FOnAmmoPercentageChanged OnAmmoPercentageChanged;
	
	virtual void SetConceal(bool bState) override;
private:
	virtual void P_Shoot(bool bIsFirstWeapon) override;
	virtual void P_Reload(int32 OtherAmmoSource) override;
	
	virtual void BeginPlay() override;
	virtual void ShootProjectile(EPoolObject ProjectileName) override;
};