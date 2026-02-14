#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "TricksterGrenadeProjectionNotify.generated.h"

UCLASS(DisplayName = "Grenade Projection Notify", Category = "Custom Notifies | Trickster")
class POSSESSIONGAME_API UTricksterGrenadeProjectionNotify : public UAnimNotify_PlayMontageNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
