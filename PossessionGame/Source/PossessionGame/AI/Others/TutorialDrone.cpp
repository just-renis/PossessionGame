#include "TutorialDrone.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Navigation/DroneWaypoint.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Global Utilities/UI Actors/TutorialDialogue.h"
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
	PlaySound(ESound::DroneBuzz);
	SetupAttachments();
	SetupDelegates();
	RetrieveDroneWaypoints();
	PlayDroneAnimation();
	DelegateManager->OnGameModeFinished.AddUObject(this, &ATutorialDrone::EnableCheckDistanceToTarget);
	UKismetSystemLibrary::K2_SetTimer(this, "SpinPropellers", 0.05f, true);
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
	UKismetSystemLibrary::K2_SetTimer(this, "MoveToLocation", 0.015f, true);
}

void ATutorialDrone::EnableCheckDistanceToTarget(const int32 InIndex)
{
	DistanceToTargetIndex = InIndex;
	UKismetSystemLibrary::K2_SetTimer(this, "CheckDistanceToTarget", 0.05f, true);
}

void ATutorialDrone::EnableCheckDistanceToTarget(const EGameMode InGameMode)
{
	DistanceToTargetIndex = -1;
	UKismetSystemLibrary::K2_SetTimer(this, "CheckDistanceToTarget", 0.05f, true);
}

void ATutorialDrone::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], InSoundName);
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
	DelegateManager->StartTutorialDroneMoveToLocation.BindUObject(this, &ATutorialDrone::SetDroneMoveLocation);
	DelegateManager->SendTargetToTutorialDrone.BindUObject(this, &ATutorialDrone::SetDroneLookAtLocation);
	DelegateManager->OnCheckDistanceToTutorialDrone.BindUObject(this, &ATutorialDrone::EnableCheckDistanceToTarget);
}

void ATutorialDrone::ShowRandomTip() const { (void)DelegateManager->OnShowDialogue.ExecuteIfBound(-1); }

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
		UKismetSystemLibrary::K2_ClearTimer(this, "MoveToLocation");
		switch (WaypointIndex)
		{
			case 0: SetDroneMoveLocation(1, false); break;
			case 1:
				DelegateManager->DestroyTutorialForceField.Broadcast(1);
				EnableCheckDistanceToTarget(1);
				break;
			case 2: EnableCheckDistanceToTarget(2); break;
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
				(void)DelegateManager->OnShowDialogue.ExecuteIfBound(0);
				(void)DelegateManager->StopShowingHint.ExecuteIfBound();
				UKismetSystemLibrary::K2_SetTimer(this, "LookAtTarget", 0.02f, true);
				break;
			case 1:
				(void)DelegateManager->OnShowDialogue.ExecuteIfBound(4);
				(void)DelegateManager->SpawnTutorialEnemy.ExecuteIfBound(ECharacterType::Minion);
				(void)DelegateManager->OnShowHint.ExecuteIfBound(2);
				DelegateManager->DestroyTutorialForceField.Broadcast(2);
				break;
			case 2: (void)DelegateManager->OnShowDialogue.ExecuteIfBound(5); break;
			default: ShowRandomTip();
		}
		UKismetSystemLibrary::K2_ClearTimer(this, "CheckDistanceToTarget");
	}
}

void ATutorialDrone::SpinPropellers() const
{
	if (!FirstPropeller || !SecondPropeller) return;
	FirstPropeller->AddLocalRotation(FRotator(0.f, 18.f, 0.f));
	SecondPropeller->AddLocalRotation(FRotator(0.f, 18.f, 0.f));
}
