#include "TutorialComputer.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ATutorialComputer::ATutorialComputer()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
	BeepingSound = CreateDefaultSubobject<UAudioComponent>("beepingSound");
	BeepingSound->SetAutoActivate(false);
	BeepingSound->SetupAttachment(Mesh);
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
	for (int32 i = 0; i < NumMaterials; i++)
	{
		if (const TObjectPtr<UMaterialInterface> Material = Mesh->GetMaterial(i))
		{
			if (Material->GetName().Contains("MI_ScreenTV"))
			{
				if (const TObjectPtr<UMaterialInstanceDynamic> MID = Mesh->CreateAndSetMaterialInstanceDynamic(i))
				{
					MID->SetScalarParameterValue("ScreenSwitchToError", 1.f);
					MID->SetScalarParameterValue("KeyboardColor", 0.365f);
				}
			}
		}
	}
	if (!AlertBeepingEnabled) return;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::ComputerElectrification, nullptr, GetActorLocation());
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(BeepingSound, nullptr, ESound::CriticalDamageBeep);
}
