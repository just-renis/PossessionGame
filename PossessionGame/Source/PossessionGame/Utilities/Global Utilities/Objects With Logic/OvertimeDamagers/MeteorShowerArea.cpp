#include "MeteorShowerArea.h"
#include "PossessionGame/Enums/EPoolObject.h"

void AMeteorShowerArea::BeginPlay()
{
	Super::BeginPlay();
	ObjectName = EPoolObject::WraithMeteorShowerArea;
}

void AMeteorShowerArea::DealOvertimeDamage()
{
	
}
