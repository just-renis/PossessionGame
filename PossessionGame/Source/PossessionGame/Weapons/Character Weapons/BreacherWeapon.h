#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "BreacherWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ABreacherWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	virtual void P_Shoot() override;
	virtual void P_Reload(const int32 InOtherAmmoSource) override;
	virtual void P_Setup() override;

	virtual void BeginPlay() override;
};
