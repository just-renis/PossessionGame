#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "BreacherWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ABreacherWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
	
	ABreacherWeapon();

	virtual void AI_Reload() override;
	virtual void AI_Shoot() override;

	virtual void P_Shoot(const EPoolObject InProjectileName) override;
	virtual void P_Reload(const int32 InOtherAmmoSource) override;
	virtual void P_Setup() override;

	virtual void BeginPlay() override;
	virtual void ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, bool bFromFirstWeapon) override;
};
