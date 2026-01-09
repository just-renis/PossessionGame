#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "WraithHandsAnimInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UWraithHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()

	virtual void OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted) override;
};
