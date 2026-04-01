#include "TricksterGrenadeLaunchNotify.h"
#include "PossessionGame/AI/Enemies/Characters/Trickster.h"

void UTricksterGrenadeLaunchNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	const TObjectPtr<AActor> Owner = MeshComp->GetOwner();
	if (!Owner) return;
	const TObjectPtr<ATrickster> Proprietor = Cast<ATrickster>(Owner);
	if (!Proprietor) return;
	if (Proprietor->IsAsyncAssetStillAllowedToBeUsed()) Proprietor->AI_LaunchPoisonGrenade();
}
