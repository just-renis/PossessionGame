#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "GravitationalPullArea.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AGravitationalPullArea : public AOvertimeDamager
{
	GENERATED_BODY()

	AGravitationalPullArea();

	UPROPERTY()
	TObjectPtr<UAudioComponent> PassiveSfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ItselfVfx;

	virtual void BeginPlay() override;
	virtual void DealOvertimeDamage() override;
	virtual void Disable(const bool bHide) override;
	virtual void Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason) override;
};
