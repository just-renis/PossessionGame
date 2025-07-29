#include "Vent.h"

AVent::AVent()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AVent::BeginPlay()
{
	Super::BeginPlay();
	if (Mesh && VentAnim) Mesh->PlayAnimation(VentAnim, false);
}
