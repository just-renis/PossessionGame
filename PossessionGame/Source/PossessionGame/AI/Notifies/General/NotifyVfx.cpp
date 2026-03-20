#include "NotifyVfx.h"
#include "PossessionGame/AI/Enemies/Animation Instances/SamuraiAnimationInstance.h"

void UNotifyVfx::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !MeshComp->GetAnimInstance()) return;
	Super::Notify(MeshComp, Animation, EventReference);
	if (const TObjectPtr<USamuraiAnimationInstance> SamuraiAnimInstance = Cast<USamuraiAnimationInstance>(MeshComp->GetAnimInstance())) SamuraiAnimInstance->PlaySlashVfx(VfxName, SlashIndex);
}
