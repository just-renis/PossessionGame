#include "SamuraiHandsDashSlashEnd.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void USamuraiHandsDashSlashEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	const TObjectPtr<UWorld> World = MeshComp->GetWorld();
	if (!World) return;
	const TObjectPtr<UGameInstance> GameInstance = World->GetGameInstance();
	if (!GameInstance) return;
	(void)GameInstance->GetSubsystem<UDelegateManager>()->OnSamuraiDashSlashEnd.ExecuteIfBound();
}
