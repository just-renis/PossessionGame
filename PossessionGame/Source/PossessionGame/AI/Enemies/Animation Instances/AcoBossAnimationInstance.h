#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "AcoBossAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UAcoBossAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()
public:
	void SetFlyingState(const bool bState) { bIsFlying = bState; }
	void SetUltraInstinctState(const bool bState) { bIsUltraInstinctActive = bState; }
	void SetUltraInstinctPose(const int32 InIndex) { UltraInstinctPoseIndex = InIndex; }
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsFlying = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsUltraInstinctActive = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	int32 UltraInstinctPoseIndex = 0;

	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted) override;
};
