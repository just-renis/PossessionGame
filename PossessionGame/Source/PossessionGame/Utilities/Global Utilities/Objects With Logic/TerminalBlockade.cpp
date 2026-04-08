#include "TerminalBlockade.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATerminalBlockade::ATerminalBlockade()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void ATerminalBlockade::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalCandleInteraction.AddUObject(this, &ATerminalBlockade::Interact);
}

void ATerminalBlockade::Interact(const int32 InId)
{
	if (InId != Id) return;
	GetWorldTimerManager().SetTimer(RisingHandle, this, &ATerminalBlockade::ProcessRising, 1.f / 60.f, true);
}

void ATerminalBlockade::ProcessRising()
{
	UnitsRose += RisingSpeed;
	AddActorLocalOffset(FVector::UpVector * RisingSpeed);
	if (UnitsRose < UnitsToRise) return;
	GetWorldTimerManager().ClearTimer(RisingHandle);	
}
