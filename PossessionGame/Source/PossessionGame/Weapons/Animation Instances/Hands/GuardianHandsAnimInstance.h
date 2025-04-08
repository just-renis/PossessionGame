#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "GuardianHandsAnimInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UGuardianHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()
	
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted) override;
};
