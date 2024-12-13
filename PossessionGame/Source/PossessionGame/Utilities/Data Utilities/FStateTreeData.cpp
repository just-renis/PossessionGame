#include "FStateTreeData.h"
#include "GameplayTagContainer.h"
#include "PossessionGame/AI/StateTrees/CustomStateTreeComponent.h"

void FStateTreeData::SetEQSPointLocation(const FVector InValue)
{
	EQSTarget->SetActorLocation(InValue);
	bIsEQSPointAvailable = true;
	StateTree->SendStateTreeEvent(FGameplayTag::RequestGameplayTag("StateTree.Abort.IsEQSPointAvailable"));
}
