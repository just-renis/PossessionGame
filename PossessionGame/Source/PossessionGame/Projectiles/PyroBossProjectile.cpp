#include "PyroBossProjectile.h"

#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

APyroBossProjectile::APyroBossProjectile()
{
	ImpactSfx = CreateDefaultSubobject<UAudioComponent>("ImpactSfx");
	ImpactSfx->SetAutoActivate(false);
	ImpactSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::PyroBossProjectileImpact, ImpactSfx);
}

void APyroBossProjectile::OnImpact(const FHitResult& Hit)
{
	DestroyThis();
	PlayImpactSound();
	const TObjectPtr<AOvertimeDamager> FirePool = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::PyroBossProjectileFirePool, GetActorLocation()));
	FirePool->Init();
	FirePool->Setup(false, Data, EDamageReason::None);
}

void APyroBossProjectile::PlayImpactSound() { PlaySound(ESound::PyroBossProjectileImpact); }
