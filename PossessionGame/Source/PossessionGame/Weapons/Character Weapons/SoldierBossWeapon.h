#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "SoldierBossWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ASoldierBossWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
};
