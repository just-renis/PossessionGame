#include "TerminalDevice.h"
#include "Components/PointLightComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

ATerminalDevice::ATerminalDevice()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	RootComponent = Mesh;
	Light->SetupAttachment(RootComponent);
	Light->SetAttenuationRadius(1500.f);
	Light->SetLightFColor(FColor(33, 155, 199));
}

void ATerminalDevice::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalHackRoomFinish.AddUObject(this, &ATerminalDevice::SetupProcessTimer);
}

void ATerminalDevice::SetupProcessTimer(const EGameMode InTerminalHackRoomMode)
{
	if (TerminalHackRoomMode != InTerminalHackRoomMode) return;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTerminalHackRoomFinish.RemoveAll(this);
	GetWorldTimerManager().SetTimer(ProcessLightIntensityHandle, this, &ATerminalDevice::ProcessLightIntensity, 0.025f, true);
}

void ATerminalDevice::ProcessLightIntensity()
{
	CurrentIntensity -= 1.125f;
	Light->SetLightFColor(FColor(149, 185, 199));
	Light->SetIntensity(CurrentIntensity);
	if (CurrentIntensity <= 5.f) GetWorldTimerManager().ClearTimer(ProcessLightIntensityHandle);
}
