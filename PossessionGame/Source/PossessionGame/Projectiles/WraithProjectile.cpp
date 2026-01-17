#include "WraithProjectile.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AWraithProjectile::AWraithProjectile()
{
	ExplosionSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ExplosionSfx"));
	ExplosionSfx->SetAutoActivate(false);
	ExplosionSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::WraithShootExplode, ExplosionSfx);
}

void AWraithProjectile::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich)
{
	if (BulletTrailComponent) BulletTrailComponent->SetHiddenInGame(false);
	Super::Init(InProjectileData, Rotation, InWhich);
}

void AWraithProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	SpawnFirstProjectileImpactVfx(Hit);
}

void AWraithProjectile::DestroyThis()
{
	if (!ExplosionVfx) AssetsManager->AttachVfx(EVfx::WraithBulletExplosion, nullptr, &ExplosionVfx, Mesh, FName("FpsTrailSocket"));
	else ExplosionVfx->Activate(true);
	bIsActive = false;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnTimeDilationChanged.RemoveAll(this);
	DisableProjectileComponents();
	if (BulletTrailComponent) BulletTrailComponent->SetHiddenInGame(true);
	DealAreaDamage();
	GetWorldTimerManager().SetTimer(ExplosionVfxHandle, this, &AWraithProjectile::ReturnObjectToPoolAfterExplosionVfxFinished, ExplosionLength);
}

void AWraithProjectile::ReturnObjectToPoolAfterExplosionVfxFinished()
{
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(ProjectileData.ProjectileName, this);
}

void AWraithProjectile::DealAreaDamage()
{
	Super::DealAreaDamage();
	PlaySound(ESound::WraithShootExplode);
}
