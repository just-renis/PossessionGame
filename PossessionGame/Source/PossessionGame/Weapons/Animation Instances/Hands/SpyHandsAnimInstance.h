#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "SpyHandsAnimInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API USpyHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()
public:
	void SetInvincibleState(const bool bState) { bIsInvincible = bState; }
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bIsInvincible = false;

	virtual void OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted) override;
};
