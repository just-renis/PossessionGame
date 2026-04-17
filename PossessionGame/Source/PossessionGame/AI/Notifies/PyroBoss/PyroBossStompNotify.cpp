#include "PyroBossStompNotify.h"
#include "PossessionGame/AI/Enemies/Characters/PyroBoss.h"

void UPyroBossStompNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	const TObjectPtr<AActor> Owner = MeshComp->GetOwner();
	if (!Owner) return;
	const TObjectPtr<APyroBoss> Proprietor = Cast<APyroBoss>(Owner);
	if (!Proprietor) return;
	if (Proprietor->IsAsyncAssetStillAllowedToBeUsed()) Proprietor->StompHitTheGround();
}
