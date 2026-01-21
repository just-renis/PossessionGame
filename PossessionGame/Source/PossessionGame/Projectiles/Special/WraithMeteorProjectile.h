#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "WraithMeteorProjectile.generated.h"

class UCodexProjectileData;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AWraithMeteorProjectile : public APoolObject
{
	GENERATED_BODY()

	AWraithMeteorProjectile();

	bool bIsHijacked = false;
	float MeteorLandTime = 1.f;
	float DefaultLifeSpan = 5.f;
	float Damage;
	float DefaultDamage;
	FTimerHandle MeteorLandHandle;
	FTimerHandle LifeSpanFinishedHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCodexProjectileData> AI_ProjectileData;
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCodexProjectileData> P_ProjectileData;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> DeployVfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	TObjectPtr<UCodexProjectileData> GetProjectileData() const { return bIsHijacked ? P_ProjectileData : AI_ProjectileData; }

	virtual void InitWithProprietor(const TObjectPtr<AEnemy> InProprietor) override;
	void ReturnToPool();
	void Landed();
	void PlaySound(const ESound InSoundName);
};
