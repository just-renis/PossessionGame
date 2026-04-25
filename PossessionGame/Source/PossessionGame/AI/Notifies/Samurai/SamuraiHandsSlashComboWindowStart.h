#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SamuraiHandsSlashComboWindowStart.generated.h"

UCLASS(DisplayName = "Samurai Player Slash Combo Window Start Notify")
class POSSESSIONGAME_API USamuraiHandsSlashComboWindowStart : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
