#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "GravitationalPullArea.generated.h"

UCLASS()
class POSSESSIONGAME_API AGravitationalPullArea : public AOvertimeDamager
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void DealOvertimeDamage() override;
	virtual void Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason) override;
};
