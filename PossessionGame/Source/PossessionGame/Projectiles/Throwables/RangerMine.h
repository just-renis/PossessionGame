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
	bool GetInExplosionProcess() const { return bInExplosionProcess; }
	
	virtual void Disable(const bool bHide) override;
	void DestroyThis() const;
private:
	ARangerMine();

	bool bAlreadyLanded = false;
	bool bInExplosionProcess = false;
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
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	virtual void BeginPlay() override;
	virtual void OnImpact(const FHitResult& Hit) override;
	void DealAreaDamage();
	void Explode();
	void PlaySound(const ESound InSoundName);
	void ChangeRadiusVfx(const EUpgrade InUpgradeName, const EUpgradeAttribute InSpecialUpgradeAttributeName);
	UFUNCTION()
	void TriggeredCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
