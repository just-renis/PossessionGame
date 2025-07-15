#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "PoisonArea.generated.h"

UCLASS()
class POSSESSIONGAME_API APoisonArea : public AOvertimeDamager
{
	GENERATED_BODY()

	virtual void DealOvertimeDamage() override;
};
