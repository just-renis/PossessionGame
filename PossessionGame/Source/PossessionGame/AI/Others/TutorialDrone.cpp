#include "TutorialDrone.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Navigation/DroneWaypoint.h"

ATutorialDrone::ATutorialDrone()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DroneMesh"));
	RootComponent = Mesh;
}

void ATutorialDrone::BeginPlay()
{
	Super::BeginPlay();
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
