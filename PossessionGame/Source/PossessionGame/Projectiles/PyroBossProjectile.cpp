#include "PyroBossProjectile.h"
#include "PossessionGame/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

void APyroBossProjectile::OnImpact(const FHitResult& Hit)
{
	DestroyThis();
	PlayImpactSound();
	const TObjectPtr<AOvertimeDamager> FirePool = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::PyroBossProjectileFirePool, GetActorLocation()));
	FirePool->Init();
	FirePool->Setup(false, Data, EDamageReason::None);
}
