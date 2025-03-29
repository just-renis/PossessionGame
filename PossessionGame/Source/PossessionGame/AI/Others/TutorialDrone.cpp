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
	UKismetSystemLibrary::K2_SetTimer(this, "LookAtTarget", 0.02f, true);
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

void ATutorialDrone::SetDroneMoveLocation(const int32 InIndex)
{
	if (Waypoints.IsValidIndex(InIndex))
	{
		Dialogue->SetVisibilityState(false);
		WaypointIndex = InIndex;
		WaypointLocation = Waypoints[InIndex]->GetActorLocation();
		bIsMoving = true;
		UKismetSystemLibrary::K2_SetTimer(this, "MoveToLocation", 0.015f, true);
	}
}

void ATutorialDrone::EnableCheckDistanceToTarget(const int32 InIndex)
{
	DistanceToTargetIndex = InIndex;
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

void ATutorialDrone::MoveToLocation()
{
	const FVector Direction = (WaypointLocation - GetActorLocation()).GetSafeNormal();
	const FVector NewLocation = GetActorLocation() + Direction * 200.f * 0.015f;
	SetActorLocation(NewLocation);
	if (FVector::DistSquared(GetActorLocation(), WaypointLocation) < FMath::Square(10.f))
	{
		bIsMoving = false;
		Dialogue->SetVisibilityState(true);
		switch (WaypointIndex)
		{
			case 0:
				(void)DelegateManager->DestroyTutorialForceField.ExecuteIfBound();
				EnableCheckDistanceToTarget(1);
				break;
			case 1: EnableCheckDistanceToTarget(2); break;
			default: ;
		}
		UKismetSystemLibrary::K2_ClearTimer(this, "MoveToLocation");
	}
}

void ATutorialDrone::LookAtTarget()
{
	if (!Target) return;
	FRotator TargetRotation;
	TargetRotation.Yaw = ((bIsMoving ? WaypointLocation : Target->GetActorLocation()) - GetActorLocation()).GetSafeNormal2D().Rotation().Yaw - 90.f;
	FRotator NewRotation;
	NewRotation.Yaw = FMath::FixedTurn(GetActorRotation().Yaw, TargetRotation.Yaw, 1.8f);
	NewRotation.Pitch = GetActorRotation().Pitch;
	NewRotation.Roll = GetActorRotation().Roll;
	SetActorRotation(NewRotation);
}

void ATutorialDrone::CheckDistanceToTarget()
{
	if (!Target) return;
	TArray DistanceToTarget = { 25000.f, 40000.f, 40000.f };
	if (FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) < DistanceToTarget[DistanceToTargetIndex])
	{
		switch (DistanceToTargetIndex)
		{
			case 0:
				(void)DelegateManager->OnShowDialogue.ExecuteIfBound(0);
				(void)DelegateManager->StopShowingHint.ExecuteIfBound();
				break;
			case 1:
				(void)DelegateManager->OnShowDialogue.ExecuteIfBound(4);
				(void)DelegateManager->SpawnTutorialEnemy.ExecuteIfBound(ECharacterType::Minion);
				(void)DelegateManager->OnShowHint.ExecuteIfBound(2);
				break;
			case 2: (void)DelegateManager->OnShowDialogue.ExecuteIfBound(5); break;
			default: ;
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
