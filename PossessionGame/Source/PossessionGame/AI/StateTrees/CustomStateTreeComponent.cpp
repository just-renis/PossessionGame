#include "CustomStateTreeComponent.h"

TObjectPtr<UCustomStateTreeComponent> UCustomStateTreeComponent::GetTaskStateTree(EStateTreeType Id)
{
	if (TaskStateTrees.Num() < 1) return nullptr;
	// might need to check if contains an ID in future.
	return TaskStateTrees[Id];
}

void UCustomStateTreeComponent::Cleanup()
{
	Super::Cleanup();
	Destroy();
}

void UCustomStateTreeComponent::SetStateTree(TObjectPtr<UStateTree>& InStateTree)
{
	StateTreeRef.SetStateTree(InStateTree);
	InitializeComponent();
}

void UCustomStateTreeComponent::RegisterTaskStateTree(EStateTreeType Id, TObjectPtr<UCustomStateTreeComponent>& TaskStateTree)
{
	TaskStateTrees.Add(Id, TaskStateTree);
}

void UCustomStateTreeComponent::Destroy()
{
	TaskStateTrees.Empty();
}
