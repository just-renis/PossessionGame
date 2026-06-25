#include "Elevator.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

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
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnElevatorTriggered.BindUObject(this, &AElevator::InitiateRising);
	for (const TObjectPtr<AActor> SpawnLocation : EnemiesSpawnLocations) SpawnLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

void AElevator::InitiateRising()
{
	GetWorldTimerManager().SetTimer(RisingHandle, this, &AElevator::ProcessRising, 0.05f, true);
}

void AElevator::ProcessRising()
{
	SetActorLocation(GetActorLocation() + FVector::UpVector * RisingSpeed);
	if (WardenSpawnLocation) WardenSpawnLocation->SetActorLocation(WardenSpawnLocation->GetActorLocation() + FVector::UpVector * RisingSpeed);
	UnitsRose += RisingSpeed;
	if (UnitsRose >= UnitsToRise)
	{
		UnitsRose = 0.f;
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnElevatorFinishedRising.ExecuteIfBound();
		GetWorldTimerManager().ClearTimer(RisingHandle);
	}
}
