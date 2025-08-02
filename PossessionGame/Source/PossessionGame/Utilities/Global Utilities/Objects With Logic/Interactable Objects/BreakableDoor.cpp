#include "BreakableDoor.h"

ABreakableDoor::ABreakableDoor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}
