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

	bool GetIsPlayerProjectile() { return ProjectileData.bIsPlayerProjectile; }

	void SetupProjectile(const FProjectileData& InProjectileData, const FVector& Scale);
	void SetDirection(const FVector Direction) const;
protected:
	FProjectileData ProjectileData;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	void PlaySound(const ESound InSoundName);
	virtual void PlayImpactSound() {}
	virtual void LoadSounds() {}
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
private:
	bool bHasBeenPooled = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UProjectileMovementComponent> MovementComponent;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BulletTrailComponent;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	
	virtual void BeginPlay() override;
	void SetupData();
	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale) override;
	void DealAreaDamage();
	void AttachBulletTrail(EVfx VfxName);
	void AttachAndLoadVfxs();
	void SpawnOrLoadBulletImpactVfx(EVfx VfxName, const FVector& InLocation, const FRotator& InRotation);
	void SpawnBulletImpactVfx(EVfx VfxName, const FVector& InLocation, const FRotator& InRotation);
	void DealDamageToSentientActor(const TObjectPtr<AActor>& OtherActor, const FHitResult& Hit);
	UFUNCTION()
	void DestroyThis();
};
