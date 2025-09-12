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
	bool GetIsSetShootOnly() const { return bShootOnly; }
	bool GetIsDirectionForward() const { return bIsDirectionForward; }
	
	void SetIsEnabled(const bool bState) { bEnabled = bState; }
	void SetIdleAnimationState(const bool bState) { bIdleAnimationEnabled = bState; }
	void SetLookAtLocation(const FVector& InLookAtLocation) { LookAtLocation = InLookAtLocation; }
	void SetShootOnly(const bool bState) { bShootOnly = bState; }
	void SetMovementDirectionForward(const bool bState);
private:
	bool bIsDirectionForward = true;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bEnabled = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIdleAnimationEnabled = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bShootOnly = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float MovementDirectionPlayRate = 1.f;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	FVector LookAtLocation = FVector::ZeroVector;
};
