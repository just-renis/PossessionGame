#include "ForceField.h"
#include "Components/AudioComponent.h"
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
	MID_Material = ForceField->CreateAndSetMaterialInstanceDynamic(0);
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
	DissolveSfx->Sound ? DissolveSfx->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(DissolveSfx, nullptr, ESound::ForceFieldDissolve);
	GetWorldTimerManager().SetTimer(DissolveHandle, this, &AForceField::ProcessDissolveForceField, 0.025f, true);
}

void AForceField::ProcessDissolveForceField()
{
	if (!MID_Material) return;
	PatternMaxOpacity -= 0.0125f;
	PatternOpacityDivisor -= 0.1f;
	MID_Material->SetScalarParameterValue("PatternMaxOpacity", PatternMaxOpacity); 
	MID_Material->SetScalarParameterValue("PatternOpacityDivisor", PatternOpacityDivisor);
	if (PatternOpacityDivisor > 0.f) return;
	DestroyThis();
}
