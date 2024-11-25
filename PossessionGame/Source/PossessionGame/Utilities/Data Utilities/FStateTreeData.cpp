#include "FStateTreeData.h"
#include "GameplayTagContainer.h"
#include "PossessionGame/AI/StateTrees/CustomStateTreeComponent.h"

void FStateTreeData::SetEQSPointLocation(FVector Value)
{
	EQSTarget->SetActorLocation(Value);
	bIsEQSPointAvailable = true;
	StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.Abort.IsEQSPointAvailable"));
}
