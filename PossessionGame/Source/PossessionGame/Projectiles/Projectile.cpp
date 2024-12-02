#include "Projectile.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_VfxContainer.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = Mesh;
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("movementComponent"));
	MovementComponent->InitialSpeed = 2000.f;
	MovementComponent->MaxSpeed = 2000.0f;
	MovementComponent->ProjectileGravityScale = 0.0f;
}

void AProjectile::SetupProjectile(const FProjectileData& InProjectileData, const FVector& Location, const FRotator& Rotation, const FVector& Scale)
{
	SetActorScale3D(Scale);
	SetActorLocationAndRotation(Location, Rotation);
	ProjectileData = InProjectileData;
	Mesh->SetCollisionObjectType(ProjectileData.bIsPlayerProjectile ? ECC_GameTraceChannel1 : ECC_GameTraceChannel2);
	SetupData();
	if (!bHasBeenPooled)
	{
		AttachAndLoadVfxs();
		bHasBeenPooled = true;
	}
}

void AProjectile::SetDirection(FVector Direction) { MovementComponent->Velocity = Direction * MovementComponent->MaxSpeed; }

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this || OtherActor == nullptr) return;
	DestroyThis();
	SpawnOrLoadBulletImpactVfx(AssetsManager->GetVfxName(ProjectileData.ProjectileName, EVfxType::Impact), Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	if (ProjectileData.bProjectileDealsAreaDamage)
	{
		DealAreaDamage();
		return;
	}
	if (!OtherActor->ActorHasTag("Enemy") && !OtherActor->ActorHasTag("Destructible")) return;
	if (OtherActor->ActorHasTag("Destructible"))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileData.WeaponDamage + ProjectileData.Damage, Hit.ImpactNormal, Hit, nullptr, this, UDamageType::StaticClass());
		return;
	}
	const bool bIsEnemyHit = Mesh->GetCollisionObjectType() == ECC_GameTraceChannel2 && Cast<AEnemy>(OtherActor)->GetIsPossessed();
	const bool bIsPlayerHit = Mesh->GetCollisionObjectType() == ECC_GameTraceChannel1;
	if (bIsEnemyHit || bIsPlayerHit)
	{
		const float DamageMultiplier = Hit.BoneName == "Head" ? 2.0f : 1.0f;
		UGameplayStatics::ApplyPointDamage(OtherActor, (ProjectileData.WeaponDamage + ProjectileData.Damage) * DamageMultiplier, Hit.ImpactNormal, Hit, nullptr, this, UDamageType::StaticClass());
	}
}

void AProjectile::DestroyThis()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "DestroyThis");
	if (BulletTrailComponent) BulletTrailComponent->Deactivate();
	MovementComponent->Deactivate();
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(ProjectileData.ProjectileName, this);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
	Mesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::SetupData()
{
	UKismetSystemLibrary::K2_SetTimer(this, "DestroyThis", 0.01f, false, false, ProjectileData.LifeSpan);
	MovementComponent->InitialSpeed = ProjectileData.InitialSpeed;
	MovementComponent->MaxSpeed = ProjectileData.MaxSpeed;
	MovementComponent->ProjectileGravityScale = ProjectileData.GravityScale;
}

void AProjectile::SetIsInUse(const FProjectileData& InProjectileData, const FVector& Location, const FRotator& Rotation, const FVector& Scale)
{
	SetupProjectile(InProjectileData, Location, Rotation, Scale);
	if (!InProjectileData.bBoostUpward) SetDirection(FMath::VRandCone(Rotation.Vector(), FMath::DegreesToRadians(ProjectileData.GetCalcSpread())));
	else SetDirection(FMath::VRandCone((Rotation.Vector() + FVector(0.f, 0.f, InProjectileData.BoostUpwardValue)).GetSafeNormal(), FMath::DegreesToRadians(ProjectileData.GetCalcSpread())));
	if (BulletTrailComponent) BulletTrailComponent->Activate();
	MovementComponent->Activate();
}

void AProjectile::DealAreaDamage()
{
	TArray<TObjectPtr<AActor>> IgnoredActors;
	if (!ProjectileData.bCanBeDamagedByOwnProjectile)
	{
		if (!ProjectileData.bIsPlayerProjectile) IgnoredActors = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetAllEnemies();
		else IgnoredActors = { ProjectileData.GetProprietor() };
	}
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), ProjectileData.WeaponDamage + ProjectileData.Damage, ProjectileData.RadiusMinimumDamage,
	GetActorLocation(), ProjectileData.DamageInnerRadius, ProjectileData.DamageOuterRadius, ProjectileData.DamageFalloff, UDamageType::StaticClass(),
	IgnoredActors, this, nullptr, ECC_GameTraceChannel4);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 100.f, 12, FColor::Green, false, 5.f);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 500.f, 12, FColor::Red, false, 5.f);
}

void AProjectile::AttachBulletTrail(EVfx VfxName)
{
	BulletTrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(AssetsManager->GetVfxData(VfxName)->GetVfxEffect().Get(), Mesh, FName("BulletTrailSocket"),
	FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
}

void AProjectile::AttachAndLoadVfxs()
{
	EVfx VfxName = AssetsManager->GetVfxName(ProjectileData.ProjectileName, EVfxType::Trail);
	if (VfxName != EVfx::None)
	{
		if (AssetsManager->GetVfxData(VfxName)->GetVfxEffect().IsValid()) AttachBulletTrail(VfxName);
		else AssetsManager->GetVfxData(VfxName)->LoadVfx(GetWorld(), [this, VfxName] { AttachBulletTrail(VfxName); });
	}
}

void AProjectile::SpawnOrLoadBulletImpactVfx(EVfx VfxName, const FVector& InLocation, const FRotator& InRotation)
{
	if (AssetsManager->GetVfxData(VfxName)->GetVfxEffect().IsValid()) SpawnBulletImpactVfx(VfxName, InLocation, InRotation);
	else AssetsManager->GetVfxData(VfxName)->LoadVfx(GetWorld(), [this, VfxName, InLocation, InRotation] { SpawnBulletImpactVfx(VfxName, InLocation, InRotation); });
}

void AProjectile::SpawnBulletImpactVfx(EVfx VfxName, const FVector& InLocation, const FRotator& InRotation)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AssetsManager->GetVfxData(VfxName)->GetVfxEffect().Get(), InLocation, InRotation);
}
