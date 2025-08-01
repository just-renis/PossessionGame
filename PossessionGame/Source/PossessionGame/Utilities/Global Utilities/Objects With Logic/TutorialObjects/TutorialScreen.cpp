#include "TutorialScreen.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATutorialScreen::ATutorialScreen()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

void ATutorialScreen::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAlertStart.AddUObject(this, &ATutorialScreen::ChangeToAlertedMode);
}

void ATutorialScreen::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAlertStart.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ATutorialScreen::ChangeToAlertedMode() const
{
	const int32 NumMaterials = Mesh->GetNumMaterials();
	for (int32 i = 0; i < NumMaterials; i++)
	{
		if (const TObjectPtr<UMaterialInterface> Material = Mesh->GetMaterial(i))
		{
			if (Material->GetName().Contains("MI_DeathLogoSheet"))
			{
				if (const TObjectPtr<UMaterialInstanceDynamic> MID = Mesh->CreateAndSetMaterialInstanceDynamic(i))
				{
					MID->SetScalarParameterValue("Opacity", 1.f);
				}
			}
		}
	}
}
