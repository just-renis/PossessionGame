#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "BreacherHandsAnimInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UBreacherHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()

	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted) override;
};
