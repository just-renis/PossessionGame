#include "ForceField.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/TutorialManager.h"

AForceField::AForceField()
{
	ForceField = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("forceField"));
	RootComponent = ForceField;
}

void AForceField::BeginPlay()
{
	Super::BeginPlay();
	MID_Material = ForceField->CreateAndSetMaterialInstanceDynamic(0);
	if (!Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsTutorialFinished())
	{
		GetWorld()->GetSubsystem<UTutorialManager>()->OnShowHint.AddUObject(this, &AForceField::StartDeploymentOfForceField);
	}
	GetGameInstance()->GetSubsystem<UDelegateManager>()->StartForceFieldDissolve.AddUObject(this, &AForceField::StartDissolveForceField);
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

void AForceField::StartDissolveForceField(const int32 InIndex)
{
	if (ForceFieldIndex != InIndex) return;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->StartForceFieldDissolve.RemoveAll(this);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySoundAtLocation(ESound::ForceFieldDissolve, GetActorLocation());
	GetWorldTimerManager().SetTimer(DissolveHandle, this, &AForceField::ProcessDissolveForceField, 0.025f, true);
}

void AForceField::ProcessDissolveForceField()
{
	if (!MID_Material) return;
	PatternMaxOpacity -= 0.0125f;
	PatternOpacityDivisor -= 0.1f;
	DeploymentPercent -= 0.05f;
	MID_Material->SetScalarParameterValue("PatternMaxOpacity", PatternMaxOpacity); 
	MID_Material->SetScalarParameterValue("PatternOpacityDivisor", PatternOpacityDivisor);
	MID_Material->SetScalarParameterValue("DeploymentPercent", DeploymentPercent);
	if (PatternOpacityDivisor > 0.f) return;
	DestroyThis();
}

void AForceField::StartDeploymentOfForceField()
{
	if (++HintIndex < 5) return;
	GetWorld()->GetSubsystem<UTutorialManager>()->OnShowHint.RemoveAll(this);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySoundAtLocation(ESound::RangerDeployShield, GetActorLocation());
	GetWorldTimerManager().SetTimer(DissolveHandle, this, &AForceField::ProcessDeploymentOfForceField, 0.025f, true);
}

void AForceField::ProcessDeploymentOfForceField()
{
	if (!MID_Material) return;
	PatternMaxOpacity += 0.0125f;
	PatternOpacityDivisor += 0.1f;
	DeploymentPercent += 0.05f;
	MID_Material->SetScalarParameterValue("PatternMaxOpacity", PatternMaxOpacity); 
	MID_Material->SetScalarParameterValue("PatternOpacityDivisor", PatternOpacityDivisor);
	MID_Material->SetScalarParameterValue("DeploymentPercent", DeploymentPercent);
	if (PatternOpacityDivisor >= 2.f) GetWorldTimerManager().ClearTimer(DissolveHandle);
}
