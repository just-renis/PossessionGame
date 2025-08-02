#include "TutorialDrone.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Navigation/DroneWaypoint.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/TutorialDialogue.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ATutorialDrone::ATutorialDrone()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DroneMesh"));
	FirstPropeller = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("firstPropellerMesh"));
	SecondPropeller = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("secondPropellerMesh"));
	RootComponent = Mesh;
	Sounds.Add(ESound::DroneBuzz, CreateDefaultSubobject<UAudioComponent>(TEXT("droneBuzzSound")));
	Sounds[ESound::DroneBuzz]->SetAutoActivate(false);
	Sounds[ESound::DroneBuzz]->SetupAttachment(Mesh);
}

void ATutorialDrone::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(LookAtTargetHandle, this, &ATutorialDrone::LookAtTarget, 0.02f, true);
	PlaySound(ESound::DroneBuzz);
	SetupAttachments();
	RetrieveDroneWaypoints();
	SetupDelegates();
	PlayDroneAnimation();
	GetWorldTimerManager().SetTimer(SpinPropellersHandle, this, &ATutorialDrone::SpinPropellers, 0.05f, true);
}

void ATutorialDrone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(SpinPropellersHandle);
	GetWorldTimerManager().ClearTimer(MoveToLocationHandle);
	GetWorldTimerManager().ClearTimer(CheckDistanceToTargetHandle);
	GetWorldTimerManager().ClearTimer(LookAtTargetHandle);
	Super::EndPlay(EndPlayReason);
}

void ATutorialDrone::PlayDroneAnimation() const
{
	if (Mesh && DroneAnimation) Mesh->PlayAnimation(DroneAnimation, true);
}

void ATutorialDrone::RetrieveDroneWaypoints()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADroneWaypoint::StaticClass(), Actors);
	for (TObjectPtr<AActor> Actor : Actors) Waypoints.Add(Cast<ADroneWaypoint>(Actor));
}

void ATutorialDrone::SetDroneMoveLocation(const int32 InIndex, const bool bInstantMove)
{
	bMoveInstantly = bInstantMove;
	Dialogue->SetVisibilityState(false);
	WaypointIndex = InIndex;
	for (int32 i = 0; i < Waypoints.Num(); i++) if (Waypoints[i]->GetOrder() == InIndex) WaypointLocation = Waypoints[i]->GetActorLocation();
	bIsMoving = true;
	GetWorldTimerManager().SetTimer(MoveToLocationHandle, this, &ATutorialDrone::MoveToLocation, 0.015f, true);
}

void ATutorialDrone::EnableCheckDistanceToTarget(const int32 InIndex)
{
	DistanceToTargetIndex = InIndex;
	GetWorldTimerManager().SetTimer(CheckDistanceToTargetHandle, this, &ATutorialDrone::CheckDistanceToTarget, 0.05f, true);
}

void ATutorialDrone::EnableCheckDistanceToTarget(const EGameMode InGameMode)
{
	DistanceToTargetIndex = -1;
	GetWorldTimerManager().SetTimer(CheckDistanceToTargetHandle, this, &ATutorialDrone::CheckDistanceToTarget, 0.05f, true);
}

void ATutorialDrone::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], nullptr, InSoundName);
}

void ATutorialDrone::SetupAttachments()
{
	Dialogue = GetWorld()->SpawnActor<ATutorialDialogue>(DialogueClass);
	Dialogue->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "DialogueSocket");
	FirstPropeller->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, "FirstPropellerSocket");
	SecondPropeller->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, "SecondPropellerSocket");
}

void ATutorialDrone::SetupDelegates()
{
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
}

void ATutorialDrone::MoveToLocation()
{
	if (bIsRotating) return;
	if (!bMoveInstantly)
	{
		const FVector Direction = (WaypointLocation - GetActorLocation()).GetSafeNormal();
		const FVector NewLocation = GetActorLocation() + Direction * MovementSpeed;
		SetActorLocation(NewLocation);
	}
	else SetActorLocation(WaypointLocation);
	if (FVector::DistSquared(GetActorLocation(), WaypointLocation) < FMath::Square(10.f))
	{
		bIsMoving = false;
		GetWorldTimerManager().ClearTimer(MoveToLocationHandle);
		switch (WaypointIndex)
		{
			case 0: SetDroneMoveLocation(1, false); break;
			case 1:
				DelegateManager->DestroyTutorialForceField.Broadcast(1);
				EnableCheckDistanceToTarget(1);
				break;
			case 2:
				if (Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot()->GetIsTutorialFinished()) EnableCheckDistanceToTarget(3);
				else EnableCheckDistanceToTarget(2);
			default: ;
		}
	}
}

void ATutorialDrone::LookAtTarget()
{
	if (!Target) return;
	FRotator TargetRotation;
	TargetRotation.Yaw = ((bIsMoving ? WaypointLocation : Target->GetActorLocation()) - GetActorLocation()).GetSafeNormal2D().Rotation().Yaw - 90.f;
	if (FMath::Abs(FMath::FindDeltaAngleDegrees(GetActorRotation().Yaw, TargetRotation.Yaw)) > 2.f)
	{
		bIsRotating = true;
		FRotator NewRotation;
		NewRotation.Yaw = FMath::FixedTurn(GetActorRotation().Yaw, TargetRotation.Yaw, RotationSpeed);
		NewRotation.Pitch = GetActorRotation().Pitch;
		NewRotation.Roll = GetActorRotation().Roll;
		SetActorRotation(NewRotation);
	}
	else bIsRotating = false;
}

void ATutorialDrone::CheckDistanceToTarget()
{
	if (!Target) return;
	if (FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) < 60000.f)
	{
		switch (DistanceToTargetIndex)
		{
			case 0:
				break;
			case 1:
				DelegateManager->DestroyTutorialForceField.Broadcast(2);
				break;
		}
		GetWorldTimerManager().ClearTimer(CheckDistanceToTargetHandle);
	}
}

void ATutorialDrone::SpinPropellers() const
{
	if (!FirstPropeller || !SecondPropeller) return;
	FirstPropeller->AddLocalRotation(FRotator(0.f, 18.f, 0.f));
	SecondPropeller->AddLocalRotation(FRotator(0.f, 18.f, 0.f));
}
