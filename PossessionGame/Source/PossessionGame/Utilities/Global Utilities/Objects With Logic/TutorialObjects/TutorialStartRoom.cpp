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
	GetWorldTimerManager().ClearTimer(BlinkingHandle);
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
			MID_Lamp = Mesh->CreateAndSetMaterialInstanceDynamic(i);
			MID_Lamp->SetScalarParameterValue("Desaturation", -10.f);
			MID_Lamp->SetScalarParameterValue("HueChange", 0.45f);
		}
	}
	GetWorldTimerManager().SetTimer(BlinkingHandle, this, &ATutorialStartRoom::Blink, 0.5f, true);
}

void ATutorialStartRoom::Blink()
{
	if (!MID_Lamp) return;
	bIsLampsOn = !bIsLampsOn;
	MID_Lamp->SetScalarParameterValue("PulsingOn", bIsLampsOn ? 1.f : 0.f);
}
