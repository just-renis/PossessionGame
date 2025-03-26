#include "ForceField.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

AForceField::AForceField()
{
	ForceField = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("forceField"));
	RootComponent = ForceField;
}

void AForceField::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->DestroyTutorialForceField.BindUObject(this, &AForceField::DestroyThis);
}

void AForceField::DestroyThis() { Destroy(); }
