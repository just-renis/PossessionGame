#include "Elevator.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AElevator::AElevator()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	WardenSpawnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WardenSpawnMesh"));
	WardenSpawnMesh->SetupAttachment(RootComponent);
	ElevatorRisingSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ElevatorRisingSfx"));
	ElevatorRisingSfx->SetAutoActivate(false);
	ElevatorRisingSfx->SetupAttachment(RootComponent);
}

void AElevator::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnElevatorTriggered.AddUObject(this, &AElevator::InitiateRising);
	for (const TObjectPtr<AActor> SpawnLocation : EnemiesSpawnLocations) SpawnLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

void AElevator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnElevatorTriggered.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void AElevator::InitiateRising()
{
	ElevatorRisingSfx->Sound ? ElevatorRisingSfx->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(ElevatorRisingSfx, nullptr, ESound::ElevatorRising);
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
