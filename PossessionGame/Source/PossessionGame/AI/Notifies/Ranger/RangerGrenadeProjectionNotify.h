#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "RangerGrenadeProjectionNotify.generated.h"

UCLASS(DisplayName = "Grenade Projection Notify", Category = "Custom Notifies | Ranger")
class POSSESSIONGAME_API URangerGrenadeProjectionNotify : public UAnimNotify_PlayMontageNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
