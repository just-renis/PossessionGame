#include "Projectile.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EVfxType.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"

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

void AProjectile::SetupProjectile(const FProjectileData& InProjectileData, const FVector& Scale, const int32 InWhich)
{
	SetActorScale3D(Scale);
	ProjectileData = InProjectileData;
	Mesh->SetCollisionObjectType(ProjectileData.bIsPlayerProjectile ? ECC_GameTraceChannel1 : ECC_GameTraceChannel2);
	SetupData(ProjectileData.bIsPlayerProjectile, InWhich);
	if (!bHasBeenPooled)
	{
		if (const EVfx VfxName = AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, EVfxType::Trail); VfxName != EVfx::None)
		{
			AssetsManager->AttachOrLoadVfx(AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, EVfxType::Trail), &BulletTrailComponent, Mesh, FName("BulletTrailSocket"));
		}
		bHasBeenPooled = true;
	}
}

void AProjectile::SetDirection(const FVector InDirection) const { MovementComponent->Velocity = InDirection * MovementComponent->MaxSpeed; }

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	AssetsManager = GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>();
	Mesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::PlaySound(const ESound InSoundName) { Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : AssetsManager->ConfigureSound(Sounds[InSoundName], InSoundName); }

void AProjectile::OnImpact(const FHitResult& Hit)
{
	DestroyThis();
	SpawnFirstProjectileImpactVfx(Hit);
	SpawnSecondProjectileImpactVfx(Hit);
	PlayImpactSound();
}

void AProjectile::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale, const int32 InWhich)
{
	Super::Init(InProjectileData, Rotation, Scale, InWhich);
	SetupProjectile(InProjectileData, Scale, InWhich);
	SetDirection(FMath::VRandCone(Rotation.Vector(), FMath::DegreesToRadians(ProjectileData.GetCalcSpread())));
	if (BulletTrailComponent) BulletTrailComponent->Activate();
	MovementComponent->Activate();
}

void AProjectile::DisableProjectileComponents() const
{
	if (BulletTrailComponent) BulletTrailComponent->Deactivate();
	MovementComponent->Deactivate();
}

void AProjectile::SpawnProjectileImpactVfx(const FHitResult& InHit, const bool bFirstImpact, const TMap<FName, FNiagaraAttribute>& InAttributes)
{
	AssetsManager->SpawnOrLoadVfx(AssetsManager->GetProjectileVfxName(ProjectileData.ProjectileName, bFirstImpact ? EVfxType::Impact : EVfxType::SecondaryImpact), InHit.ImpactPoint, InHit.ImpactNormal.Rotation(), InAttributes);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this || OtherActor == nullptr) return;
	OnImpact(Hit);
	if (ProjectileData.bProjectileDealsAreaDamage)
	{
		DealAreaDamage();
		return;
	}
	if (OtherActor->ActorHasTag("Destructible"))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, Hit.ImpactNormal, Hit, nullptr, this, UDamageType::StaticClass());
		return;
	}
	if (!OtherActor->ActorHasTag("Enemy")) return;
	if (Mesh->GetCollisionObjectType() == ECC_GameTraceChannel1) // If enemy got hit.
	{
		DealDamageToSentientActor(OtherActor, Hit);
		return;
	}
	if (Mesh->GetCollisionObjectType() == ECC_GameTraceChannel2 && Cast<AEnemy>(OtherActor)->GetIsPossessed()) DealDamageToSentientActor(OtherActor, Hit); // If player got hit.
}

void AProjectile::AI_SetupUpgradesAppliance(const bool bIsPlayerProjectile)
{
	Damage += GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayerProjectile, ProjectileData.ProjectileName, EUpgradeAttribute::FlatDamage);
	const float DamagePercentage = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayerProjectile, ProjectileData.ProjectileName, EUpgradeAttribute::DamagePercentage);
	if (DamagePercentage > 0.f) Damage += ProjectileData.Damage * (DamagePercentage / 100.f);
}

void AProjectile::SetupData(const bool bIsPlayerProjectile, const int32 InWhich)
{
	UKismetSystemLibrary::K2_SetTimer(this, "DestroyThis", ProjectileData.LifeSpan, false);
	MovementComponent->InitialSpeed = ProjectileData.GetMultiProjectileData(ProjectileData.Amount).Data[InWhich].InitialSpeed;
	MovementComponent->MaxSpeed = ProjectileData.GetMultiProjectileData(ProjectileData.Amount).Data[InWhich].MaxSpeed;
	MovementComponent->ProjectileGravityScale = ProjectileData.GetMultiProjectileData(ProjectileData.Amount).Data[InWhich].GravityScale;
	Damage = ProjectileData.Damage;
	AI_SetupUpgradesAppliance(bIsPlayerProjectile);
}

void AProjectile::DealAreaDamage()
{
	TArray<TObjectPtr<AActor>> IgnoredActors;
	if (!ProjectileData.bCanBeDamagedByOwnProjectile)
	{
		if (!ProjectileData.bIsPlayerProjectile) IgnoredActors = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetAllEnemies();
		else IgnoredActors = { ProjectileData.GetProprietor() };
	}
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), Damage, ProjectileData.RadiusMinimumDamage,
	GetActorLocation(), ProjectileData.DamageInnerRadius, ProjectileData.DamageOuterRadius, ProjectileData.DamageFalloff, UDamageType::StaticClass(),
	IgnoredActors, this, nullptr, ECC_GameTraceChannel4);
//	DrawDebugSphere(GetWorld(), GetActorLocation(), 100.f, 12, FColor::Green, false, 3.f);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 500.f, 12, FColor::Red, false, 3.f);
}

void AProjectile::DealDamageToSentientActor(const TObjectPtr<AActor>& OtherActor, const FHitResult& Hit)
{
	const float DamageMultiplier = Hit.BoneName == "head" ? 2.f : 1.f;
	UGameplayStatics::ApplyPointDamage(OtherActor, Damage * DamageMultiplier, Hit.ImpactNormal, Hit, nullptr, this, UDamageType::StaticClass());
}

void AProjectile::SpawnFirstProjectileImpactVfx(const FHitResult& Hit) { SpawnProjectileImpactVfx(Hit, true); }

void AProjectile::SpawnSecondProjectileImpactVfx(const FHitResult& Hit) { SpawnProjectileImpactVfx(Hit, false); }

void AProjectile::DestroyThis()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "DestroyThis");
	DisableProjectileComponents();
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(ProjectileData.ProjectileName, this);
}
