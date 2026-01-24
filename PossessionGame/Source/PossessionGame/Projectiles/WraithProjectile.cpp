#include "WraithProjectile.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/EPlayerLevelUpgradeId.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

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

void AWraithProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) { SpawnFirstProjectileImpactVfx(Hit); }

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

void AWraithProjectile::DealAreaDamage(const float InOtherDamageSource)
{
	float TrueDamage = Damage;
	const TObjectPtr<USaveSlot> SaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot();
	const bool bIsUpgradeSelected = SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::FifthOptionB);
	TrueDamage += DefaultDamage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevelAllGlobalDamageBonus();
	TrueDamage += DefaultDamage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardDamage) * 0.01f;
	TrueDamage += DefaultDamage * (bIsUpgradeSelected ? SaveSlot->GetPlayerLevel() * 0.05f : 0.f);
	TrueDamage += DefaultDamage * GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetPoisonAreaStackBoost(); // Trickster Purple Variation Damage Boost.
	const bool bIsUpgradeSelectedEightOptionA = SaveSlot->IsPlayerLevelUpgradeSelected(EPlayerLevelUpgradeId::EighthOptionA);
	if (bIsUpgradeSelectedEightOptionA && FMath::RandRange(1, 100) < 26) TrueDamage *= 2.f;
	Super::DealAreaDamage(TrueDamage);
	PlaySound(ESound::WraithShootExplode);
}
