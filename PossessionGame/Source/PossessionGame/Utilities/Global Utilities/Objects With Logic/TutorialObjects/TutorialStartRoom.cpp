#include "TutorialStartRoom.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"

ATutorialStartRoom::ATutorialStartRoom()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

void ATutorialStartRoom::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<UTutorialManager>()->OnAlertStart.AddUObject(this, &ATutorialStartRoom::ChangeToAlertState);
}

void ATutorialStartRoom::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<UTutorialManager>()->OnAlertStart.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ATutorialStartRoom::ChangeToAlertState()
{
	const int32 NumMaterials = Mesh->GetNumMaterials();
	for (int32 i = 0; i < NumMaterials; i++)
	{
		const TObjectPtr<UMaterialInterface> Material = Mesh->GetMaterial(i);
		if (!Material) continue;
		if (Material->GetName().Contains("MI_Lamp"))
		{
			const TObjectPtr<UMaterialInstanceDynamic> MID_Lamp = Mesh->CreateAndSetMaterialInstanceDynamic(i);
			MID_Lamp->SetScalarParameterValue("Desaturation", -10.f);
			MID_Lamp->SetScalarParameterValue("HueChange", 0.45f);
			MID_Lamp->SetScalarParameterValue("PulseOnOff", 1.f);
		}
	}
}
