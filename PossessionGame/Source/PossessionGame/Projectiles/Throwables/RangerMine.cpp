#include "RangerMine.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

ARangerMine::ARangerMine()
{
	MineTriggerCollision = CreateDefaultSubobject<USphereComponent>("MineTriggerCollision");
	MineTriggerCollision->SetupAttachment(Mesh);
	MineTriggerCollision->SetCollisionProfileName(FName("TriggerPlayer"));
	MineTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MineTriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ARangerMine::TriggeredCollision);
	DeploySfx = CreateDefaultSubobject<UAudioComponent>(TEXT("MineImpactSfx"));
	DeploySfx->SetAutoActivate(false);
	DeploySfx->SetupAttachment(Mesh);
	ExplosionSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("MineExplosionSfx"));
	ExplosionSfx->SetAutoActivate(false);
	ExplosionSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::RangerDeployMine, DeploySfx);
	Sounds.Add(ESound::RangerDetonateMine, ExplosionSfx);
}

void ARangerMine::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerMineRadius, [this]() -> bool { return bAlreadyLanded; }, &RadiusVfx, Mesh);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerMineImpact, [this]() -> bool { return bAlreadyLanded; }, &ImpactVfx, Mesh);
}

void ARangerMine::OnImpact(const FHitResult& Hit)
{
	Super::OnImpact(Hit);
	bAlreadyLanded = true;
	PlaySound(ESound::RangerDeployMine);
	if (ImpactVfx) ImpactVfx->Activate();
	if (RadiusVfx)
	{
		TMap<FName, FNiagaraAttribute> Attributes;
		const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::RangerAbility3, EUpgradeAttribute::Size, true);
		const float CollisionRadius = DefaultCollisionRadius + DefaultCollisionRadius * RadiusIncrease * 0.01f;
		Attributes.Add(FName("Mesh Uniform Scale"), FNiagaraAttribute(true, CollisionRadius / DefaultCollisionRadius));
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->UpdateVfxAttributes(RadiusVfx, Attributes);
		MineTriggerCollision->SetSphereRadius(CollisionRadius);
		RadiusVfx->Activate();
	}
	MineTriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ARangerMine::DestroyThis() const
{
	if (ImpactVfx) ImpactVfx->DeactivateImmediate();
	if (RadiusVfx) RadiusVfx->DeactivateImmediate();
	MineTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARangerMine::DealAreaDamage()
{
	//TArray<TObjectPtr<AActor>> IgnoredActors;
	//if (!ProjectileData.bIsPlayerProjectile) IgnoredActors = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetAllEnemies();
	//else IgnoredActors = { ProjectileData.GetProprietor() };
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), Data->GetDamage(), Data->GetRadiusMinimumDamage(),
	GetActorLocation(), Data->GetDamageInnerRadius(), Data->GetDamageOuterRadius(), Data->GetDamageFalloff(), UDamageType::StaticClass(),
	TArray<AActor*>(), this, nullptr, ECC_GameTraceChannel4);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::RangerMineExplosion, nullptr, GetActorLocation());
}

void ARangerMine::Explode()
{
	DealAreaDamage();
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestRangerMineRemoval.ExecuteIfBound(this);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnRangerMineToPool(this);
	bInExplosionProcess = false;
}

void ARangerMine::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], TFunction<bool()>(), InSoundName);
}

void ARangerMine::TriggeredCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag(FName("AliveEnemy"))) return;
	bInExplosionProcess = true;
	PlaySound(ESound::RangerDetonateMine);
	DestroyThis();
	GetWorldTimerManager().SetTimer(ExplosionHandle, this, &ARangerMine::Explode, ExplosionDelay);
}
