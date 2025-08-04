#include "TutorialCapsule.h"

ATutorialCapsule::ATutorialCapsule()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	FirstRotatorMesh = CreateDefaultSubobject<UStaticMeshComponent>("FirstRotatorMesh");
	SecondRotatorMesh = CreateDefaultSubobject<UStaticMeshComponent>("SecondRotatorMesh");
	FirstRotatorMesh->SetupAttachment(RootComponent);
	SecondRotatorMesh->SetupAttachment(RootComponent);
}

void ATutorialCapsule::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(RotationHandle, this, &ATutorialCapsule::ProcessRotation, 0.025f, true);
}

void ATutorialCapsule::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(RotationHandle);
	Super::EndPlay(EndPlayReason);
}

void ATutorialCapsule::ProcessRotation() const
{
	FRotator CurrentRotation = FirstRotatorMesh->GetRelativeRotation();
	CurrentRotation.Yaw += RotationSpeedPerSec * 0.025f;
	FirstRotatorMesh->SetRelativeRotation(CurrentRotation);
	CurrentRotation = SecondRotatorMesh->GetRelativeRotation();
	CurrentRotation.Yaw -= RotationSpeedPerSec * 0.025f;
	SecondRotatorMesh->SetRelativeRotation(CurrentRotation);
}
