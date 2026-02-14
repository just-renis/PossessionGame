#include "TricksterGrenadeProjectionNotify.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EVfx.h"

void UTricksterGrenadeProjectionNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	const TObjectPtr<AActor> Owner = MeshComp->GetOwner();
	if (!Owner) return;
	const TObjectPtr<AEnemy> Proprietor = Cast<AEnemy>(Owner);
	if (!Proprietor) return;
	if (Proprietor->IsAsyncAssetStillAllowedToBeUsed()) Proprietor->AI_TriggerGrenadeProjection(EPoolObject::TricksterPoisonProjectile, EVfx::TricksterGrenadeLauncherMarker);
}
