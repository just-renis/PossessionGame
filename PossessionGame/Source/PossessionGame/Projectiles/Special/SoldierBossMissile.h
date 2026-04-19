#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "SoldierBossMissile.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API ASoldierBossMissile : public APoolObject
{
	GENERATED_BODY()

	ASoldierBossMissile();

	bool bIsActive = true;
	UPROPERTY(EditDefaultsOnly, Category = "Debug", meta = (AllowPrivateAccess = true))
	bool bEnableDebug = false;
	UPROPERTY(EditDefaultsOnly, Category = "Missile Data", meta = (AllowPrivateAccess = true))
	float Damage = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Missile Data", meta = (AllowPrivateAccess = true))
	float LandTime = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Missile Data", meta = (AllowPrivateAccess = true))
	float AreaInMeters = 4.f;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ImpactSfx;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;

	virtual void InitWithProprietor(const TObjectPtr<AEnemy> InProprietor) override;
	void Landed();
	void ReturnToPool();
};
