#include "TutorialStartRoom.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATutorialStartRoom::ATutorialStartRoom()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

void ATutorialStartRoom::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAlertStart.AddUObject(this, &ATutorialStartRoom::ChangeToAlertState);
}

void ATutorialStartRoom::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAlertStart.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ATutorialStartRoom::ChangeToAlertState() const
{
	const int32 NumMaterials = Mesh->GetNumMaterials();
	for (int32 i = 0; i < NumMaterials; i++)
	{
		const TObjectPtr<UMaterialInterface> Material = Mesh->GetMaterial(i);
		if (!Material) continue;
		if (Material->GetName().Contains("MI_Lamp"))
		{
			const TObjectPtr<UMaterialInstanceDynamic> MID = Mesh->CreateAndSetMaterialInstanceDynamic(i);
			MID->SetScalarParameterValue("Desaturation", 10.f);
			MID->SetScalarParameterValue("HueChange", 0.45f);
		}
	}
}
