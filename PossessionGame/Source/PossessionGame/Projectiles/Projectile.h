#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/PoolObject.h"
#include "Projectile.generated.h"

enum class ESound : uint8;
enum class EVfxType : uint8;
enum class EVfx : uint8;
class UPrimaryDataAssetsManager;
class UNiagaraComponent;
class IDamageable;
enum class EPoolObject : uint8;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class POSSESSIONGAME_API AProjectile : public APoolObject
{
	GENERATED_BODY()
public:
	AProjectile();

	bool GetIsPlayerProjectile() const { return ProjectileData.bIsPlayerProjectile; }

	void SetupProjectile(const FProjectileData& InProjectileData, const FVector& Scale);
	void SetDirection(const FVector Direction) const;
protected:
	FProjectileData ProjectileData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	void PlaySound(const ESound InSoundName);
	virtual void PlayImpactSound() {}
	virtual void OnImpact(const FHitResult& Hit);
	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale) override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
private:
	bool bHasBeenPooled = false;
	float Damage;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UProjectileMovementComponent> MovementComponent;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BulletTrailComponent;
	
	void AI_SetupUpgradesAppliance(const bool bIsPlayerProjectile);
	
	virtual void BeginPlay() override;
	void SetupData(const bool bIsPlayerProjectile);
	void DealAreaDamage();
	void AttachBulletTrail(EVfx VfxName);
	void AttachAndLoadVfxs();
	void SpawnBulletImpactVfx(EVfx VfxName, const FVector& InLocation, const FRotator& InRotation);
	void SpawnOrLoadBulletImpactVfx(EVfx VfxName, const FVector& InLocation, const FRotator& InRotation);
	void DealDamageToSentientActor(const TObjectPtr<AActor>& OtherActor, const FHitResult& Hit);
	UFUNCTION()
	virtual void DestroyThis();
};
