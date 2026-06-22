#include "WallLaser.h"

AWallLaser::AWallLaser()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AWallLaser::BeginPlay()
{
	Super::BeginPlay();
	MID_Material = Mesh->CreateAndSetMaterialInstanceDynamic(0);
}
