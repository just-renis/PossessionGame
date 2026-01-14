#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "WraithTrapArea.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AWraithTrapArea : public AOvertimeDamager
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	float DefaultRadius = 360.f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> RadiusVfx;

	virtual void BeginPlay() override;
	virtual void DealOvertimeDamage() override;
	virtual void Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason) override;
	virtual void RemoveTarget(const TObjectPtr<AEnemy>& InTarget) override;
};
