#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "SamuraiHandsAnimInstance.generated.h"

class ASamuraiWeapon;

UCLASS()
class POSSESSIONGAME_API USamuraiHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ASamuraiWeapon> Weapon;

	bool IsCorrectMontage(UAnimMontage* Montage, EHandsAnimState State);

	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted) override;
	virtual void NativeBeginPlay() override;
};
