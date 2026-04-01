#include "SamuraiSlashNotify.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void USamuraiSlashNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	const TObjectPtr<AActor> Owner = MeshComp->GetOwner();
	if (!Owner) return;
	const TObjectPtr<AEnemy> Proprietor = Cast<AEnemy>(Owner);
	if (!Proprietor || !Proprietor->GetWeapon()) return;
	if (Proprietor->IsAsyncAssetStillAllowedToBeUsed()) Proprietor->GetWeapon()->AI_Shoot();
}
