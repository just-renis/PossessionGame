#include "ForceField.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

AForceField::AForceField()
{
	ForceField = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("forceField"));
	RootComponent = ForceField;
}

void AForceField::BeginPlay()
{
	Super::BeginPlay();
	MI_Material = ForceField->CreateAndSetMaterialInstanceDynamic(0);
	if (bIsTutorialForceField) GetGameInstance()->GetSubsystem<UDelegateManager>()->DestroyTutorialForceField.BindUObject(this, &AForceField::StartDissolveForceField);
	else GetGameInstance()->GetSubsystem<UDelegateManager>()->OnChargeTerminalModeCompletion.BindUObject(this, &AForceField::StartDissolveForceField);
}

void AForceField::DestroyThis()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "DissolveForceField");
	Destroy();
}

void AForceField::StartDissolveForceField() { UKismetSystemLibrary::K2_SetTimer(this, "DissolveForceField", 0.025f, true); }

void AForceField::DissolveForceField()
{
	MaterialOpacity -= 0.025f;
	if (MI_Material) MI_Material->SetScalarParameterValue(FName("DeploymentPercent"), MaterialOpacity);
	if (MaterialOpacity <= 0.4f) DestroyThis();
}
