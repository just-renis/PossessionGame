#include "TerminalEnergyCore.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATerminalEnergyCore::ATerminalEnergyCore()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void ATerminalEnergyCore::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalCandleInteraction.AddUObject(this, &ATerminalEnergyCore::Interact);
}

void ATerminalEnergyCore::Interact(const int32 InId)
{
	// DO SHIT
}
