#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifyFootstep.generated.h"

UCLASS()
class POSSESSIONGAME_API UAnimNotifyFootstep : public UAnimNotify
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (AllowPrivateAccess = true))
	bool bIsPlayer;
	UPROPERTY(EditAnywhere, Category = "Sound", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundBase> Sound;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
