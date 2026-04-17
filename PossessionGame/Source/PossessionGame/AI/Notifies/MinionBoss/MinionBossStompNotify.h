#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MinionBossStompNotify.generated.h"

UCLASS(DisplayName = "Minion Boss Stomp Notify")
class POSSESSIONGAME_API UMinionBossStompNotify : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
