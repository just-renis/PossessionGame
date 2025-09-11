#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CloneAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UCloneAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	bool GetIsIdleAnimationEnabled() const { return bIdleAnimationEnabled; }
	
	void SetIsEnabled(const bool bState) { bEnabled = bState; }
	void SetIdleAnimationState(const bool bState) { bIdleAnimationEnabled = bState; }
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bEnabled = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIdleAnimationEnabled = false;
};
