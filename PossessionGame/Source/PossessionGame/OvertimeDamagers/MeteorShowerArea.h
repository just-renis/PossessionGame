#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "MeteorShowerArea.generated.h"

class UCodexProjectileData;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AMeteorShowerArea : public AOvertimeDamager
{
	GENERATED_BODY()

	bool bIsHijacked = false;
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	float DefaultRadius = 360.f;
	UPROPERTY(EditDefaultsOnly, Category = "Meteor", meta = (AllowPrivateAccess = true))
	float MeteorSpawnRadiusReduction = 20.f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> DeployVfx;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;

	virtual void BeginPlay() override;
	virtual void DealOvertimeDamage() override;
	virtual void InitWithProprietor(const TObjectPtr<AEnemy> InProprietor) override;
	virtual void Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason) override;
	virtual void AddTarget(const TObjectPtr<AEnemy>& InTarget) override {}
	virtual void RemoveTarget(const TObjectPtr<AEnemy>& InTarget) override {}
	void SpawnMeteor();
};
