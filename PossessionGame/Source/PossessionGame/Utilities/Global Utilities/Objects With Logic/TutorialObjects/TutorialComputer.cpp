#include "TutorialComputer.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"

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
	GetWorld()->GetSubsystem<UTutorialManager>()->OnAlertStart.AddUObject(this, &ATutorialComputer::ChangeToAlertedMode);
}

void ATutorialComputer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<UTutorialManager>()->OnAlertStart.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ATutorialComputer::ChangeToAlertedMode() const
{
	const int32 NumMaterials = Mesh->GetNumMaterials();
	for (int32 i = 0; i < NumMaterials; i++)
	{
		const TObjectPtr<UMaterialInterface> Material = Mesh->GetMaterial(i);
		if (!Material) continue;
		const TObjectPtr<UMaterialInstanceDynamic> MID = Mesh->CreateAndSetMaterialInstanceDynamic(i);
		if (!MID) continue;
		if (Material->GetName().Contains("MI_ScreenTV")) MID->SetScalarParameterValue("ScreenSwitchToError", 1.f);
		else MID->SetScalarParameterValue("KeyboardColor", 0.365f);
	}
	if (!AlertBeepingEnabled) return;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::ComputerElectrification, nullptr, GetActorLocation());
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(BeepingSound, nullptr, ESound::CriticalDamageBeep);
	if (BeepingSound) BeepingSound->SetFloatParameter("Volume", 0.4f);
}
