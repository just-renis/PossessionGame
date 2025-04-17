#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "MinionBoss.generated.h"

UCLASS()
class POSSESSIONGAME_API AMinionBoss : public AEnemy
{
	GENERATED_BODY()

	int32 CurrentWeaponIndex = 0;
	UPROPERTY()
	TArray<TObjectPtr<AWeapon>> Weapons;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_ShootLogic() override;
	virtual void AI_Shoot() override;
	virtual void AI_SetCanShoot(const bool bState) override;
	virtual void AI_CancelShooting() override;
	virtual void AI_InitiateShooting() override;

	virtual TObjectPtr<AWeapon>& GetWeapon() override; // For getting singular weapon data only.
	
	virtual void BeginPlay() override;
	virtual void HandleDeactivation() override;
	virtual void DestroyThis() override;
};
