#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "TricksterGrenadeLaunchNotify.generated.h"

UCLASS(DisplayName = "Grenade Launch Notify", Category = "Custom Notifies | Trickster")
class POSSESSIONGAME_API UTricksterGrenadeLaunchNotify : public UAnimNotify_PlayMontageNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
