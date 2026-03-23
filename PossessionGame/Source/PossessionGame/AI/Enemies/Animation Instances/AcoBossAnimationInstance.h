#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "AcoBossAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UAcoBossAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()
public:
	void SetIsFlying(const bool bState) { bIsFlying = bState; }
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsFlying = false;
};
