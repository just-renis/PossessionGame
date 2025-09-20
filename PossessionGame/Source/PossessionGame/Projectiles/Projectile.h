#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Interfaces/DamageReasonProvider.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Projectile.generated.h"

class USphereComponent;
class UUpgradeManager;
enum class ESound : uint16;
class UPrimaryDataAssetsManager;
class UNiagaraComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class POSSESSIONGAME_API AProjectile : public APoolObject, public IDamageReasonProvider
{
	GENERATED_BODY()
public:
	AProjectile();

	bool GetIsPlayerProjectile() const { return ProjectileData.bIsPlayerProjectile; }

	void SetupProjectile(const FProjectileData& InProjectileData, const int32 InWhich);
	void SetDirection(const FVector& InDirection) const;
protected:
	float Damage;
	float DefaultDamage;
	FProjectileData ProjectileData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UProjectileMovementComponent> MovementComponent;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;
	UPROPERTY()
	TMap<FName, FNiagaraAttribute> Attributes;
	
	virtual float GetModifiedLifeSpan() const { return ProjectileData.LifeSpan; }
	virtual float GetModifiedProjectileInitialSpeed(const float InBaseInitialSpeed) const { return 0.f; }
	virtual float GetModifiedProjectileMaxSpeed(const float InBaseMaxSpeed) const { return 0.f; }

	virtual void BeginPlay() override;
	void PlaySound(const ESound InSoundName);
	virtual void PlayImpactSound() {}
	virtual void OnImpact(const FHitResult& Hit);
	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich) override;
	virtual void InitCloneProjectile(FProjectileData& InProjectileData, const FRotator& Rotation) override;
	void DisableProjectileComponents() const;
	virtual void SpawnProjectileImpactVfx(const FHitResult& InHit, const bool bFirstImpact);
	void DealAreaDamage();
	void DealDamageToSentientActor(const bool bIsPlayerProjectile, const TObjectPtr<AActor>& OtherActor, const FHitResult& Hit);
	virtual void SpawnFirstProjectileImpactVfx(const FHitResult& Hit);
	virtual void DestroyThis();
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
private:
	bool bHasBeenPooled = false;
	FVector StartLocation;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BulletTrailComponent;
	UPROPERTY()
	TObjectPtr<UUpgradeManager> UpgradeManager;
	UPROPERTY()
	FTimerHandle DestroyHandle;

	virtual EDamageReason GetDamageReason() const override;
	
	virtual float AI_UpgradeDamageAppliance() { return 0.f; }
	
	void AI_SetupUpgradesAppliance();
	
	virtual float P_UpgradeDamageAppliance() { return 0.f; }

	void P_SetupLevelUpAppliance(const int32 InLevel);
	void P_SetupUpgradesAppliance();
	virtual void P_VariationDamageAppliance() {}

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void SetupData(const bool bIsPlayerProjectile, const int32 InWhich);
	virtual void SpawnSecondProjectileImpactVfx(const FHitResult& Hit) {}
};
