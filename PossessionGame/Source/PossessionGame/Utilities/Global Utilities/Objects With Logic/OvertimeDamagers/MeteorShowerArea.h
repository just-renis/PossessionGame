#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "MeteorShowerArea.generated.h"

UCLASS()
class POSSESSIONGAME_API AMeteorShowerArea : public AOvertimeDamager
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void DealOvertimeDamage() override;
	virtual void AddTarget(const TObjectPtr<AEnemy>& InTarget) override {}
	virtual void RemoveTarget(const TObjectPtr<AEnemy>& InTarget) override {}
};
