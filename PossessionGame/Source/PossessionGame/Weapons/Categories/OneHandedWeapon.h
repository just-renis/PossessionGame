#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "OneHandedWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AOneHandedWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	AOneHandedWeapon();
protected:
	virtual void BeginPlay() override;
};