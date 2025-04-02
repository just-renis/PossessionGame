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
	if (bIsTutorialForceField) GetGameInstance()->GetSubsystem<UDelegateManager>()->DestroyTutorialForceField.BindUObject(this, &AForceField::DestroyThis);
	else GetGameInstance()->GetSubsystem<UDelegateManager>()->OnChargeTerminalModeCompletion.BindUObject(this, &AForceField::DestroyThis);
}

void AForceField::DestroyThis() { Destroy(); }
