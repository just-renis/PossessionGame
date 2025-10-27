#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Samurai.generated.h"

class ADestructible;
enum class EPlayerInputState : uint8;

UCLASS()
class POSSESSIONGAME_API ASamurai : public AEnemy
{
	GENERATED_BODY()

	TObjectPtr<ADestructible> SamuraiHat;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<ADestructible> SamuraiHatClass;

	virtual void AI_Shoot() override;
	virtual void AI_SetupWeapon() override;

	virtual void P_Setup() override;
	virtual void P_SetupColorsForUI() override;

	virtual void BeginPlay() override;
	void SetupSamuraiHat();
	virtual void HandleDeactivation() override;
	virtual void SetAnimInstance() override;
	virtual void AI_PlayShootMontage() override;
};
