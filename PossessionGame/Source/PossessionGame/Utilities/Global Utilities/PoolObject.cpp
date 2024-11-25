#include "PoolObject.h"

void APoolObject::SetIsInUse(bool bInIsPlayerObject, const FVector& Location, const FRotator& Rotation)
{
	SetActorLocation(Location);
	SetActorRotation(Rotation);
}
