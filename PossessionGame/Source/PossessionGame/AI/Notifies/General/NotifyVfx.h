#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NotifyVfx.generated.h"

class UNiagaraComponent;
enum class EVfx : uint16;

UCLASS()
class POSSESSIONGAME_API UNotifyVfx : public UAnimNotify
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Vfx", meta = (AllowPrivateAccess = true))
	int32 SlashIndex = 1;
	UPROPERTY(EditAnywhere, Category = "Vfx", meta = (AllowPrivateAccess = true))
	EVfx VfxName;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
