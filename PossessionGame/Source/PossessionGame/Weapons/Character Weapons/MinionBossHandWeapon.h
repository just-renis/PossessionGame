#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "MinionBossHandWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AMinionBossHandWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
};
