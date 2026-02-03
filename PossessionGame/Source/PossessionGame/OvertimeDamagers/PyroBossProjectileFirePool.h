#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "PyroBossProjectileFirePool.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API APyroBossProjectileFirePool : public AOvertimeDamager
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> RadiusVfx;

	virtual void BeginPlay() override;
	virtual void Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason) override;
	virtual void DealOvertimeDamage() override;
	virtual void Disable(const bool bHide) override;
};
