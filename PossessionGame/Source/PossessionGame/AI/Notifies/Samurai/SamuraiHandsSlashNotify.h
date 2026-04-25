#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SamuraiHandsSlashNotify.generated.h"

UCLASS(DisplayName = "Samurai Player Slash Notify")
class POSSESSIONGAME_API USamuraiHandsSlashNotify : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Socket", meta = (AllowPrivateAccess = true))
	bool bIsFirstSlash;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
