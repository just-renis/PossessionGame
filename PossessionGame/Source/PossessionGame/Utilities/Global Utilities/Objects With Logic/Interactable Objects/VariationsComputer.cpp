#include "VariationsComputer.h"

AVariationsComputer::AVariationsComputer()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = Mesh;
}

void AVariationsComputer::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("INTERACTED"));
	//GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalInteraction.Broadcast();
}
