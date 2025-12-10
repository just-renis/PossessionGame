#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "BreacherAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UBreacherAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()
public:
	void SetIsBashMontagePlaying(const bool bState) { bIsBashMontagePlaying = bState; }
private:
	FTimerHandle BashMontagePlayingBlendOutHandle;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsBashMontagePlaying = false;

	virtual void OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted) override;
	void SetIsBashMontagePlayingToFalse() { bIsBashMontagePlaying = false; }
};
