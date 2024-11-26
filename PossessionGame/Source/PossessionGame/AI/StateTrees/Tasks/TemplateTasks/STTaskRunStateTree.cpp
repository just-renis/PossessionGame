#include "STTaskRunStateTree.h"
#include "StateTreeExecutionContext.h"
#include "PossessionGame/AI/StateTrees/CustomStateTreeComponent.h"
#include "PossessionGame/Utilities/Data Utilities/FStateTreeData.h"

EStateTreeRunStatus USTTaskRunStateTree::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	//StateTree = StateTreeData.GetStateTreeRef()->GetTaskStateTree(Id);
	if (StateTree == nullptr) InitializeStateTree(Context);
	else StateTree->ResumeLogic(TEXT("Resuming state"));
	UE_LOG(LogTemp, Warning, TEXT("ENTER"));
	return Super::EnterState(Context, Transition);
}

void USTTaskRunStateTree::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Warning, TEXT("EXIT"));
	StateTree->StopLogic(TEXT("Exiting state"));
	Super::ExitState(Context, Transition);
}

void USTTaskRunStateTree::InitializeStateTree(FStateTreeExecutionContext& Context)
{
	StateTree = NewObject<UCustomStateTreeComponent>(Context.GetOwner());
	StateTree->SetStateTree(StateTreeAsset);
	StateTree->RegisterComponent();
//	StateTreeData.GetStateTreeRef()->RegisterTaskStateTree(Id, StateTree);
	StateTree->StartLogic();
}
