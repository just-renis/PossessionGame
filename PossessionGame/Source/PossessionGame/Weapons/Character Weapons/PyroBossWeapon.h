#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "PyroBossWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API APyroBossWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	virtual void AI_Shoot() override;

	virtual void BeginPlay() override;
};
