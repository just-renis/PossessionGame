#include "PyroBossProjectile.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void APyroBossProjectile::OnImpact(const FHitResult& Hit)
{
	DestroyThis();
	PlayImpactSound();
	const TObjectPtr<AOvertimeDamager> FirePool = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::PyroBossProjectileFirePool, GetActorLocation()));
	FirePool->Init();
	FirePool->Setup(false, Data, EDamageReason::None);
	AssetsManager->SpawnVfx(EVfx::PyroBossProjectileFirePool, nullptr, GetActorLocation());
}
