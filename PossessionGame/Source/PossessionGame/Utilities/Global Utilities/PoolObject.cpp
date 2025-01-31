#include "PoolObject.h"

void APoolObject::Init(bool bInIsPlayerObject) { SetPoolObjectState(true); }

void APoolObject::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale) { SetPoolObjectState(true); }

void APoolObject::Disable() { SetPoolObjectState(false); }

void APoolObject::SetPoolObjectState(const bool bState)
{
	SetActorEnableCollision(bState);
	SetActorHiddenInGame(!bState);
	SetActorTickEnabled(bState);
}