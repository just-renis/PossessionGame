#include "FinalBossPlatform.h"
#include "PossessionGame/Enums/ECameraShake.h"
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
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestTriggerCameraShake.ExecuteIfBound(ECameraShake::AcoDashInitiationCameraShake);
	GetWorldTimerManager().SetTimer(RisingHandle, this, &AFinalBossPlatform::ProcessRising, 0.025f, true);
}

void AFinalBossPlatform::ProcessRising()
{
	SetActorLocation(GetActorLocation() + FVector::UpVector * RisingSpeed);
	UnitsRose += RisingSpeed;
	if (UnitsRose >= UnitsToRise)
	{
		(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAcoPlatformRiseComplete.ExecuteIfBound();
		GetWorldTimerManager().ClearTimer(RisingHandle);
	}
}
