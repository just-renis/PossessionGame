#include "ForceField.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AForceField::AForceField()
{
	ForceField = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("forceField"));
	RootComponent = ForceField;
	DissolveSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("dissolveSfx"));
	DissolveSfx->SetAutoActivate(false);
	DissolveSfx->SetupAttachment(ForceField);
}

void AForceField::BeginPlay()
{
	Super::BeginPlay();
	MI_Material = ForceField->CreateAndSetMaterialInstanceDynamic(0);
	if (TutorialForceFieldIndex != 0)
	{
		if (Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsTutorialFinished()) DestroyThis();
		else GetGameInstance()->GetSubsystem<UDelegateManager>()->DestroyTutorialForceField.AddUObject(this, &AForceField::StartDissolveTutorialForceField);
	}
	else GetGameInstance()->GetSubsystem<UDelegateManager>()->OnChargeTerminalModeCompletion.BindUObject(this, &AForceField::StartDissolveForceField);
}

void AForceField::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DissolveHandle);
	Super::EndPlay(EndPlayReason);
}

void AForceField::DestroyThis()
{
	GetWorldTimerManager().ClearTimer(DissolveHandle);
	Destroy();
}

void AForceField::StartDissolveTutorialForceField(const int32 InIndex)
{
	if (TutorialForceFieldIndex != InIndex) return;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->DestroyTutorialForceField.RemoveAll(this);
	StartDissolveForceField();
}

void AForceField::StartDissolveForceField()
{
	DissolveSfx->Sound ? DissolveSfx->Play() : GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(DissolveSfx, nullptr, ESound::ForceFieldDissolve);
	GetWorldTimerManager().SetTimer(DissolveHandle, this, &AForceField::DissolveForceField, 0.025f, true);
}

void AForceField::DissolveForceField()
{
	MaterialOpacity -= 0.025f;
	if (MI_Material) MI_Material->SetScalarParameterValue(FName("DeploymentPercent"), MaterialOpacity);
	if (MaterialOpacity <= 0.4f) DestroyThis();
}
