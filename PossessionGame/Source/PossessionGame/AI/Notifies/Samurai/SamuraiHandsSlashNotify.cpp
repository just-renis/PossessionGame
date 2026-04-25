#include "SamuraiHandsSlashNotify.h"
#include "PossessionGame/Weapons/Animation Instances/Hands/SamuraiHandsAnimInstance.h"

void USamuraiHandsSlashNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !MeshComp->GetAnimInstance()) return;
	if (const TObjectPtr<USamuraiHandsAnimInstance> SamuraiHandsAnimInstance = Cast<USamuraiHandsAnimInstance>(MeshComp->GetAnimInstance())) SamuraiHandsAnimInstance->PlaySlashVfx(bIsFirstSlash);
}
