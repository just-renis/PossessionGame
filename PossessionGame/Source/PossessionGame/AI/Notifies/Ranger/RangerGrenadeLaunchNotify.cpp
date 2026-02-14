#include "RangerGrenadeLaunchNotify.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"

void URangerGrenadeLaunchNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	const TObjectPtr<AActor> Owner = MeshComp->GetOwner();
	if (!Owner) return;
	const TObjectPtr<ARanger> Proprietor = Cast<ARanger>(Owner);
	if (!Proprietor) return;
	if (Proprietor->IsAsyncAssetStillAllowedToBeUsed()) Proprietor->AI_LaunchGrenade();
}
