#include "Elevator.h"

AElevator::AElevator()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	WardenSpawnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WardenSpawnMesh"));
	RootComponent = Mesh;
	WardenSpawnMesh->SetupAttachment(RootComponent);
}

void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
}
