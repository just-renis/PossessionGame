#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "WardenBlasterWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AWardenBlasterWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	AWardenBlasterWeapon();

	bool bFirstWeaponTrigger = true;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ShootSfx;

	virtual void AI_Shoot() override;
	
	virtual void BeginPlay() override;
};
