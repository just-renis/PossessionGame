#include "RangerMine.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EPoolObject.h"
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
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerMineExplosion, [this]() -> bool { return bAlreadyExploded; }, &MineExplosionVfx);
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

void ARangerMine::DealAreaDamage()
{
	//TArray<TObjectPtr<AActor>> IgnoredActors;
	//if (!ProjectileData.bIsPlayerProjectile) IgnoredActors = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetAllEnemies();
	//else IgnoredActors = { ProjectileData.GetProprietor() };
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), Data->GetDamage(), Data->GetRadiusMinimumDamage(),
	GetActorLocation(), Data->GetDamageInnerRadius(), Data->GetDamageOuterRadius(), Data->GetDamageFalloff(), UDamageType::StaticClass(),
	TArray<AActor*>(), this, nullptr, ECC_GameTraceChannel4);
	if (MineExplosionVfx)
	{
		MineExplosionVfx->SetWorldLocation(GetActorLocation());
		UE_LOG(LogTemp, Error, TEXT("SET LOCATION %s"), *GetActorLocation().ToString());
		MineExplosionVfx->Activate();
	}
}

void ARangerMine::TriggeredCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag(FName("AliveEnemy"))) return;
	DealAreaDamage();
	bAlreadyExploded = true;
	if (MineRadiusVfx) MineImpactVfx->DeactivateImmediate();
	if (MineRadiusVfx) MineRadiusVfx->DeactivateImmediate();
	MineTriggerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorScale3D(FVector(1.f));
	CurrentScale = 1.f;
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::RangerMine, this);
}
