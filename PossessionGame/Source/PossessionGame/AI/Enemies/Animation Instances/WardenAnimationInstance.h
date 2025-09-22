#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "WardenAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UWardenAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()
public:
	void SetIsActionMontagePlaying(const bool bState) { bIsActionMontagePlaying = bState; }
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsActionMontagePlaying = false;
};
