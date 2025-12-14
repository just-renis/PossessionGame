#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "TricksterHandsAnimInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UTricksterHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()

	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted) override;
};
