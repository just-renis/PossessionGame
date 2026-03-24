#include "FinalBossPlatform.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

AFinalBossPlatform::AFinalBossPlatform()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AFinalBossPlatform::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAcoFirstPhaseFinish.BindUObject(this, &AFinalBossPlatform::SetupRisingLogic);
}

void AFinalBossPlatform::SetupRisingLogic()
{
	GetWorldTimerManager().SetTimer(RisingHandle, this, &AFinalBossPlatform::ProcessRising, 0.025f, true);
}

void AFinalBossPlatform::ProcessRising()
{
	SetActorLocation(GetActorLocation() + FVector::UpVector);
	UnitsRose++;
	if (UnitsRose >= UnitsToRise) GetWorldTimerManager().ClearTimer(RisingHandle);
}
