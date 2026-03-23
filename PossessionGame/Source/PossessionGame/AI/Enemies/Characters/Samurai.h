#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Samurai.generated.h"

class ADestructible;

UCLASS()
class POSSESSIONGAME_API ASamurai : public AEnemy
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> SamuraiHatClass;
	UPROPERTY()
	TObjectPtr<ADestructible> SamuraiHat;

	virtual void AI_SetupWeapon() override;
	virtual void AI_SetupShootLogic() override {}

	virtual void P_Setup() override;
	virtual void P_WeaponShoot() override;

	virtual void BeginPlay() override;
	void SetupSamuraiHat();
	virtual void HandleDeactivation() override;
	virtual void Landed(const FHitResult& Hit) override;
};
