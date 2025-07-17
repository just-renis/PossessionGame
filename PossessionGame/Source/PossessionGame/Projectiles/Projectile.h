#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Interfaces/DamageReasonProvider.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Projectile.generated.h"

class UUpgradeManager;
enum class ESound : uint16;
enum class EVfxType : uint8;
enum class EVfx : uint8;
class UPrimaryDataAssetsManager;
class UNiagaraComponent;
class IDamageable;
enum class EPoolObject : uint8;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class POSSESSIONGAME_API AProjectile : public APoolObject, public IDamageReasonProvider
{
	GENERATED_BODY()
public:
	AProjectile();

	bool GetIsPlayerProjectile() const { return ProjectileData.bIsPlayerProjectile; }

	void SetupProjectile(const FProjectileData& InProjectileData, const FVector& Scale, const int32 InWhich);
	void SetDirection(const FVector& InDirection) const;
protected:
	float Damage;
	float DefaultDamage;
	FProjectileData ProjectileData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;
	UPROPERTY()
	TMap<FName, FNiagaraAttribute> Attributes;

	virtual void BeginPlay() override;
	void PlaySound(const ESound InSoundName);
	virtual void PlayImpactSound() {}
	virtual void OnImpact(const FHitResult& Hit);
	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale, const int32 InWhich) override;
	void DisableProjectileComponents() const;
	void SpawnProjectileImpactVfx(const FHitResult& InHit, const bool bFirstImpact);
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
private:
	bool bHasBeenPooled = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UProjectileMovementComponent> MovementComponent;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BulletTrailComponent;
	UPROPERTY()
	TObjectPtr<UUpgradeManager> UpgradeManager;
	UPROPERTY()
	FTimerHandle DestroyHandle;

	virtual EDamageReason GetDamageReason() const override;
	
	void AI_SetupUpgradesAppliance();

	void P_SetupLevelUpAppliance(const int32 InLevel);
	void P_SetupUpgradesAppliance();
	virtual void P_VariationDamageAppliance() {}

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void SetupData(const bool bIsPlayerProjectile, const int32 InWhich);
	void DealAreaDamage();
	void DealDamageToSentientActor(const bool bIsPlayerProjectile, const TObjectPtr<AActor>& OtherActor, const FHitResult& Hit);
	virtual void SpawnFirstProjectileImpactVfx(const FHitResult& Hit);
	virtual void SpawnSecondProjectileImpactVfx(const FHitResult& Hit) {}
	virtual void DestroyThis();
};
