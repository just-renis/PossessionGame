#pragma once

#include "CoreMinimal.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "SamuraiSlashNotify.generated.h"

UCLASS(DisplayName = "Samurai Slash Notify")
class POSSESSIONGAME_API USamuraiSlashNotify : public UAnimNotify_PlayMontageNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
