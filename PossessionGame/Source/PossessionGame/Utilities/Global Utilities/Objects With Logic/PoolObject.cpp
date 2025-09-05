#include "PoolObject.h"

void APoolObject::Init(bool bInIsPlayerObject) { SetPoolObjectState(true); }

void APoolObject::Init(const float InData, FLinearColor InColor, const bool bHide) { SetPoolObjectState(true, bHide); }

void APoolObject::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich) { SetPoolObjectState(true); }

void APoolObject::Disable(const bool bHide) { SetPoolObjectState(false, bHide); }

void APoolObject::SetPoolObjectState(const bool bState, const bool bHide)
{
	SetActorEnableCollision(bState);
	if (bHide) SetActorHiddenInGame(!bState);
	SetActorTickEnabled(bState);
}