#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "SpyHandsAnimInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API USpyHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsInvisible = false;

	virtual void NativeBeginPlay() override;
	void SetIsInvisible(bool bState) { bIsInvisible = !bState; }
};
