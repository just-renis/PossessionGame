#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "WraithWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AWraithWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	virtual void P_UpdateAmmoTextHUD() override;

	virtual void BeginPlay() override;
};
