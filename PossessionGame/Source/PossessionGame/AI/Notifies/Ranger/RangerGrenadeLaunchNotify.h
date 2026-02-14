#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "RangerGrenadeLaunchNotify.generated.h"

UCLASS(DisplayName = "Grenade Launch Notify", Category = "Custom Notifies | Ranger")
class POSSESSIONGAME_API URangerGrenadeLaunchNotify : public UAnimNotify_PlayMontageNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
