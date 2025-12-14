#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "RangerHandsAnimInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API URangerHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()

	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted) override;
};
