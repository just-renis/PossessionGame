#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"
#include "TankWeaponAnimInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UTankWeaponAnimInstance : public UWeaponAnimInstance
{
	GENERATED_BODY()

	virtual void OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted) override;
};
