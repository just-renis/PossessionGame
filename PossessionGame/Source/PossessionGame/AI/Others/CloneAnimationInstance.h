#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CloneAnimationInstance.generated.h"

class AClone;

UCLASS()
class POSSESSIONGAME_API UCloneAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	void SetSpeed(const float InSpeed) { Speed = InSpeed; }
	void SetIsEnabled(const bool bState);
private:
	FVector LastVelocity;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bEnabled = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float Speed = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	float DirectionalAngle = 0.f;
	UPROPERTY()
	TObjectPtr<AClone> Proprietor;
	UPROPERTY()
	FTimerHandle UpdateDirectionalAngleHandle;

	virtual void NativeBeginPlay() override;
	void UpdateDirectionalAngle();
};
