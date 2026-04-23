#include "SamuraiHandsSlashNotify.h"
#include "PossessionGame/Weapons/Animation Instances/Hands/SamuraiHandsAnimInstance.h"

void USamuraiHandsSlashNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !MeshComp->GetAnimInstance()) return;
	Super::Notify(MeshComp, Animation, EventReference);
	if (const TObjectPtr<USamuraiHandsAnimInstance> SamuraiHandsAnimInstance = Cast<USamuraiHandsAnimInstance>(MeshComp->GetAnimInstance())) SamuraiHandsAnimInstance->PlaySlashVfx();
}
