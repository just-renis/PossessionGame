#include "SecretDoor.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ASecretDoor::ASecretDoor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = Mesh;
}

void ASecretDoor::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSecretDoorOpened.BindUObject(this, &ASecretDoor::DestroyThis);
}

void ASecretDoor::DestroyThis() { Destroy(); }
