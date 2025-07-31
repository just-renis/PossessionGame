#pragma once

#include "CoreMinimal.h"
#include "TutorialBot.h"
#include "Animation/AnimInstance.h"
#include "TutorialBotAnimationInstance.generated.h"

UCLASS()
class POSSESSIONGAME_API UTutorialBotAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	void SetLookAtLocation(const FVector& InLookAtLocation) { LookAtLocation = InLookAtLocation; }
	void SetIsControlRigEnabled(const bool bState) { bIsControlRigEnabled = bState; }
private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bIsControlRigEnabled = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	FVector LookAtLocation = FVector::ZeroVector;
};
