#pragma once

#include "CoreMinimal.h"
#include "Destructible.h"
#include "TankShield.generated.h"

UCLASS()
class POSSESSIONGAME_API ATankShield : public ADestructible
{
	GENERATED_BODY()

	virtual void InitWithProprietor(const TObjectPtr<AEnemy> InProprietor) override;
	virtual void SetThisLifeSpan() override;
	virtual void SetUpgradesApplication() override;
};
