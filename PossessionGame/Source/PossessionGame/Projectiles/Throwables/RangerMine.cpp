#include "RangerMine.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

ARangerMine::ARangerMine()
{
	MineTriggerCollision = CreateDefaultSubobject<USphereComponent>("MineTriggerCollision");
	MineTriggerCollision->SetupAttachment(Mesh);
	MineTriggerCollision->SetCollisionProfileName(FName("TriggerPlayer"));
	MineTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MineTriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ARangerMine::TriggeredCollision);
}

void ARangerMine::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerMineRadius, [this]() -> bool { return bAlreadyLanded; }, &MineRadiusVfx, Mesh);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerMineImpact, [this]() -> bool { return bAlreadyLanded; }, &MineImpactVfx, Mesh);
}

void ARangerMine::OnImpact(const FHitResult& Hit)
{
	Super::OnImpact(Hit);
	bAlreadyLanded = true;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::RangerDeployMine);
	if (MineRadiusVfx) MineImpactVfx->Activate();
	if (MineRadiusVfx) MineRadiusVfx->Activate();
	MineTriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ARangerMine::DestroyThis() const
{
	if (MineRadiusVfx) MineImpactVfx->DeactivateImmediate();
	if (MineRadiusVfx) MineRadiusVfx->DeactivateImmediate();
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

void ARangerMine::TriggeredCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag(FName("AliveEnemy"))) return;
	DealAreaDamage();
	DestroyThis();
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestRangerMineRemoval.ExecuteIfBound(this);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnRangerMineToPool(this);
}
