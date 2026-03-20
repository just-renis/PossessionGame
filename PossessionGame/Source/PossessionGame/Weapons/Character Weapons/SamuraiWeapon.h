#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "SamuraiWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ASamuraiWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()

	virtual void AI_Shoot() override;

	virtual void P_Shoot(const EPoolObject InProjectileName) override;
	virtual void P_UpdateAmmoTextHUD() override;

	virtual void BeginPlay() override;
	virtual void ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, const bool bFromFirstWeapon) override;
	void HandleMeleeStrike();
};
