#include "TerminalCandle.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATerminalCandle::ATerminalCandle()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	BallMesh->SetupAttachment(Mesh, FName("BallSocket"));
}

void ATerminalCandle::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalCandleInteraction.AddUObject(this, &ATerminalCandle::Interact);
}

void ATerminalCandle::Interact(const int32 InId)
{
	if (InId != Id) return;
	// DO COLOR CHANGES
}
