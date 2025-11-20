#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "RangerMine.generated.h"

enum class EUpgrade : uint8;
enum class EUpgradeAttribute : uint8;
enum class ESound : uint16;
class USphereComponent;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ARangerMine : public AThrowable
{
	GENERATED_BODY()
public:
	virtual void Disable(const bool bHide) override;
	void DestroyThis() const;
private:
	ARangerMine();

	bool bAlreadyLanded = false;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrade", meta = (AllowPrivateAccess = true))
	float SpecialUpgradeDamageMultiplier = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion", meta = (AllowPrivateAccess = true))
	float ExplosionDelay = 0.75f;
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	float DefaultCollisionRadius = 12.f;
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> MineTriggerCollision;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> RadiusVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ImpactVfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeploySfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ExplosionSfx;
	UPROPERTY()
	FTimerHandle ExplosionHandle;
	
	float UpgradeAppliance() const;

	virtual void BeginPlay() override;
	virtual void OnImpact(const FHitResult& Hit) override;
	void DealAreaDamage();
	void Explode();
	void ChangeRadiusVfx(const EUpgrade InUpgradeName, const EUpgradeAttribute InSpecialUpgradeAttributeName);
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	UFUNCTION()
	void TriggeredCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
