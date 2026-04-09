#include "TerminalEnergyCore.h"
#include "NiagaraComponent.h"
#include "Components/PointLightComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ATerminalEnergyCore::ATerminalEnergyCore()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	BallMesh->SetupAttachment(RootComponent);
	LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	LightComponent->SetupAttachment(RootComponent);
}

void ATerminalEnergyCore::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::TerminalCoreEnergy, nullptr, &EnergyCoreVfx, BallMesh, FName("VfxSocket"));
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalCandleInteraction.AddUObject(this, &ATerminalEnergyCore::Interact);
}

void ATerminalEnergyCore::Interact(const int32 InId)
{
	ColorSwitchParamIndex++;
	if (EnergyCoreVfx) EnergyCoreVfx->SetColorParameter(FName("Color"), VfxColors[ColorSwitchParamIndex - 1]);
	if (const TObjectPtr<UMaterialInstanceDynamic> MID_BallEnergyCore = BallMesh->CreateDynamicMaterialInstance(0)) MID_BallEnergyCore->SetVectorParameterValue(FName("Color"), BallColors[ColorSwitchParamIndex - 1]);
	if (const TObjectPtr<UMaterialInstanceDynamic> MID_EnergyCore = Mesh->CreateDynamicMaterialInstance(0)) MID_EnergyCore->SetScalarParameterValue(FName("ColorSwitch"), ColorSwitchParamIndex * 0.25f);
	if (LightComponent) LightComponent->SetLightColor(LightColors[ColorSwitchParamIndex - 1]);
}
