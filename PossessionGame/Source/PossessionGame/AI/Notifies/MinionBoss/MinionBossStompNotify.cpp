#include "MinionBossStompNotify.h"
#include "PossessionGame/AI/Enemies/Characters/MinionBoss.h"

void UMinionBossStompNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	const TObjectPtr<AActor> Owner = MeshComp->GetOwner();
	if (!Owner) return;
	const TObjectPtr<AMinionBoss> Proprietor = Cast<AMinionBoss>(Owner);
	if (!Proprietor) return;
	if (Proprietor->IsAsyncAssetStillAllowedToBeUsed()) Proprietor->StompHitTheGround();
}
