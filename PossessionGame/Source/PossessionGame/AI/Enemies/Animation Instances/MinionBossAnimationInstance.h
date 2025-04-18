#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "MinionBossAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UMinionBossAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()
public:
	void SetStompIsPlaying(const bool bState) { bStompIsPlaying = bState; }
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bStompIsPlaying = false;
	
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted) override;
};
