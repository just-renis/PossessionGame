#include "ForceField.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
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
	if (bAppearByGameModeInitiation)
	{
		SetMaterialValuesToInvisible();
		ForceField->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGameModeInitiator.AddUObject(this, &AForceField::AppearForceField);
	}
	if (bForTerminalHackRoom)
	{
		SetMaterialValuesToInvisible();
		ForceField->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->SendHackTerminalRoomMode.AddUObject(this, &AForceField::DeployOnTerminalHackInitiation);
		GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalHackRoomFinish.AddUObject(this, &AForceField::DissolveOnTerminalHackFinish);
	}
	else GetGameInstance()->GetSubsystem<UDelegateManager>()->StartForceFieldDissolve.AddUObject(this, &AForceField::StartDissolveForceField);
	if (GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished()) return;
	GetWorld()->GetSubsystem<UTutorialManager>()->OnShowHint.AddUObject(this, &AForceField::StartDeploymentOfForceField);
	SetMaterialValuesToInvisible();
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

void AForceField::AppearForceField(const EGameMode InGameMode)
{
	if (TerminalRoomMode != InGameMode) return;
	ForceField->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StartDeploymentOfForceField();
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
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished())
	{
		if (++HintIndex < 5) return;
		GetWorld()->GetSubsystem<UTutorialManager>()->OnShowHint.RemoveAll(this);
	}
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

void AForceField::DeployOnTerminalHackInitiation(const EGameMode InTerminalRoomMode)
{
	if (TerminalRoomMode != InTerminalRoomMode) return;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->SendHackTerminalRoomMode.RemoveAll(this);
	ForceField->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StartDeploymentOfForceField();
}

void AForceField::DissolveOnTerminalHackFinish(const EGameMode InTerminalRoomMode)
{
	if (TerminalRoomMode != InTerminalRoomMode) return;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalHackRoomFinish.RemoveAll(this);
	StartDissolveForceField(-1);
}

void AForceField::SetMaterialValuesToInvisible() const
{
	if (!MID_Material) return;
	MID_Material->SetScalarParameterValue("PatternMaxOpacity", PatternMaxOpacity); 
	MID_Material->SetScalarParameterValue("PatternOpacityDivisor", PatternOpacityDivisor);
	MID_Material->SetScalarParameterValue("DeploymentPercent", DeploymentPercent);
}
