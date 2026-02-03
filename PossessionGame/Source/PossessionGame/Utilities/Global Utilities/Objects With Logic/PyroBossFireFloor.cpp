#include "PyroBossFireFloor.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

APyroBossFireFloor::APyroBossFireFloor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void APyroBossFireFloor::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->TriggerFireFloors.AddUObject(this, &APyroBossFireFloor::TriggerFire);
}

void APyroBossFireFloor::TriggerFire(const TArray<int32>& InIds) const
{
	if (!InIds.Contains(Id)) return;
	UE_LOG(LogTemp, Error, TEXT("TRIGGGGGGGER THE FIREEEEEEEEEEEEEEEEEEEEEEE %i"), Id);
}
