#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "SpyWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ASpyWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
	
	virtual void AI_Shoot() override;
	
	virtual void P_Shoot(const EPoolObject InProjectileName) override;
	virtual void P_Reload() override;
	virtual void P_Setup() override;
	
	virtual void BeginPlay() override;
	virtual void HitScanLogic() override;
};