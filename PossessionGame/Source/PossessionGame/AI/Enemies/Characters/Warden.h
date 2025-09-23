#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Warden.generated.h"

UCLASS()
class POSSESSIONGAME_API AWarden : public AEnemy
{
	GENERATED_BODY()

	AWarden();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> BeamWeapon;

	virtual void AI_SetupWeapon() override;

	virtual void BeginPlay() override;
};
