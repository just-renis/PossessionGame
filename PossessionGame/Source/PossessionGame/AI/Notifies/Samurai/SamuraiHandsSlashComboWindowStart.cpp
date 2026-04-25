#include "SamuraiHandsSlashComboWindowStart.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void USamuraiHandsSlashComboWindowStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	const TObjectPtr<UWorld> World = MeshComp->GetWorld();
	if (!World) return;
	const TObjectPtr<UGameInstance> GameInstance = World->GetGameInstance();
	if (!GameInstance) return;
	(void)GameInstance->GetSubsystem<UDelegateManager>()->OnSamuraiSlashComboStateChange.ExecuteIfBound(true);
}
