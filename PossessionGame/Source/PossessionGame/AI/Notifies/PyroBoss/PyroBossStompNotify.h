#pragma once

#include "CoreMinimal.h"
#include "PyroBossStompNotify.generated.h"

UCLASS(DisplayName = "Pyro Boss Stomp Notify")
class POSSESSIONGAME_API UPyroBossStompNotify : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Camera Shake", meta = (AllowPrivateAccess = true))
	bool bFirstCameraShake = false;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
