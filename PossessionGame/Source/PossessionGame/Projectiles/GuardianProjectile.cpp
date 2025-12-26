#include "GuardianProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Weapons/Character Weapons/GuardianWeapon.h"

float AGuardianProjectile::P_UpgradeDamageAppliance() { return DefaultDamage * GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetGuardianTimeStopSpecialBoost(); }

void AGuardianProjectile::BeginPlay()
{
	Super::BeginPlay();
	bIsFirstSpecialBounceUpgradeTaken = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::GuardianAbility3, EUpgradeAttribute::Special1);
	if (!bIsFirstSpecialBounceUpgradeTaken) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSpecialUpgradeAdded.AddUObject(this, &AGuardianProjectile::SpecialUpgradeTaken);
}

void AGuardianProjectile::SpecialUpgradeTaken(const EUpgrade InUpgradeName, const EUpgradeAttribute InSpecialUpgradeAttributeName)
{
	bIsFirstSpecialBounceUpgradeTaken = InUpgradeName == EUpgrade::GuardianAbility3 && InSpecialUpgradeAttributeName == EUpgradeAttribute::Special1;
	if (bIsFirstSpecialBounceUpgradeTaken) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSpecialUpgradeAdded.RemoveAll(this);
}

void AGuardianProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bIsFirstSpecialBounceUpgradeTaken || ProjectileData.bIsPlayerProjectile)
	{
		Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
		return;
	}
	if (OtherActor == this || OtherActor == nullptr) return;
	if (++CurrentBounceAmount > MaxBounceAmount) DestroyThis();
	SpawnFirstProjectileImpactVfx(Hit);
	if (OtherActor->ActorHasTag("Destructible"))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, (Hit.Location - GetActorLocation()).GetSafeNormal() * 2.f, Hit, ProjectileData.GetProprietor() ? ProjectileData.GetProprietor()->GetController() : nullptr, this, UDamageType::StaticClass());
		return;
	}
	if (!OtherActor->ActorHasTag("Enemy")) return;
	const bool bIsPlayerProjectile = Mesh->GetCollisionObjectType() == ECC_GameTraceChannel1;
	if (bIsPlayerProjectile)
	{
		DealDamageToSentientActor(bIsPlayerProjectile, OtherActor, Hit);
		return;
	}
	const bool bIsEnemyProjectile = Mesh->GetCollisionObjectType() == ECC_GameTraceChannel2;
	if (bIsEnemyProjectile && Cast<AEnemy>(OtherActor)->GetIsHijacked()) DealDamageToSentientActor(bIsPlayerProjectile, OtherActor, Hit);
}

void AGuardianProjectile::SetBounceState(const bool bState) const { MovementComponent->bShouldBounce = bState; }

void AGuardianProjectile::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich)
{
	CustomTimeDilation = InProjectileData.GetProprietor()->CustomTimeDilation;
	MovementComponent->SetComponentTickInterval(1.f / 30.f / CustomTimeDilation);
	Super::Init(InProjectileData, Rotation, InWhich);
	if (!bIsFirstSpecialBounceUpgradeTaken) return;
	SetBounceState(!InProjectileData.bIsPlayerProjectile);
}

void AGuardianProjectile::DestroyThis()
{
	CurrentBounceAmount = 0;
	Super::DestroyThis();
}
