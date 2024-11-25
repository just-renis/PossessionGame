#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "RangerHandsAnimInstance.generated.h"

DECLARE_DELEGATE_OneParam(FOnGrenadeLaunchEnd, bool);

UCLASS()
class POSSESSIONGAME_API URangerHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()
public:
	FOnGrenadeLaunchEnd OnGrenadeLaunchEnd;
private:
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted) override;
};
