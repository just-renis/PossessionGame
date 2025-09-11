#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CloneAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UCloneAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	void SetIsEnabled(const bool bState) { bEnabled = bState; }
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bEnabled = false;
};
