#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "MinionBossHandWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AMinionBossHandWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	virtual void AI_Shoot() override;

	virtual void BeginPlay() override;
};
