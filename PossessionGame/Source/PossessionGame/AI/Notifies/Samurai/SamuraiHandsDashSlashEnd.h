#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SamuraiHandsDashSlashEnd.generated.h"

UCLASS(DisplayName = "Samurai Player Dash Slash End Notify")
class POSSESSIONGAME_API USamuraiHandsDashSlashEnd : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
