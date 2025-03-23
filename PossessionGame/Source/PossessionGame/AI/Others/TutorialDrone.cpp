#include "TutorialDrone.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Navigation/DroneWaypoint.h"
#include "PossessionGame/Utilities/Global Utilities/TutorialDialogue.h"

ATutorialDrone::ATutorialDrone()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DroneMesh"));
	RootComponent = Mesh;
}

void ATutorialDrone::BeginPlay()
{
	Super::BeginPlay();
	Dialogue = GetWorld()->SpawnActor<ATutorialDialogue>(DialogueClass, FVector::ZeroVector, FRotator::ZeroRotator);
	Dialogue->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "DialogueSocket");
	if (DroneDialogueTexts.IsValidIndex(0)) Dialogue->StartDialogue(DroneDialogueTexts[0]);
	RetrieveDroneWaypoints();
	PlayDroneAnimation();
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
