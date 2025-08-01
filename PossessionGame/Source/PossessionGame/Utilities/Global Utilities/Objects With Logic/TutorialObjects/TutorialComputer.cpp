#include "TutorialComputer.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATutorialComputer::ATutorialComputer()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

void ATutorialComputer::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAlertStart.AddUObject(this, &ATutorialComputer::ChangeToAlertedMode);
}

void ATutorialComputer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAlertStart.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ATutorialComputer::ChangeToAlertedMode() const
{
	const int32 NumMaterials = Mesh->GetNumMaterials();
	for (int32 Index = 0; Index < NumMaterials; Index++)
	{
		if (const TObjectPtr<UMaterialInterface> Material = Mesh->GetMaterial(Index))
		{
			if (Material->GetName().Contains("MI_ScreenTV"))
			{
				if (const TObjectPtr<UMaterialInstanceDynamic> MID = Mesh->CreateAndSetMaterialInstanceDynamic(Index))
				{
					MID->SetScalarParameterValue("ScreenSwitchToError", 1.f);
					MID->SetScalarParameterValue("KeyboardColor", 0.365f);
				}
			}
		}
	}
}
