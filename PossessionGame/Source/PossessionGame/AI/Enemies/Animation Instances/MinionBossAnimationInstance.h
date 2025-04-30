#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "MinionBossAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UMinionBossAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()
public:
	void SetIsActionMontagePlaying(const bool bState) { bIsActionMontagePlaying = bState; }
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsActionMontagePlaying = false;
	
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted) override;
};
